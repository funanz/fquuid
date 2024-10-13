#include <array>
#include <atomic>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <span>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace fquuid
{
    class not_implemented : public std::exception {};

    class ops_measure
    {
        using clock = std::chrono::high_resolution_clock;

        std::string name_;
        double time_;

    public:
        using measure_fn = std::function<void(std::stop_token, uint_fast64_t&)>;

        ops_measure(const std::string& name, double time)
            : name_(name), time_(time) {}

        void measure(measure_fn fn) const {
            uint_fast64_t ops_count = 0;

            auto thread_begin = clock::now();
            auto thread_end = thread_begin;
            std::exception_ptr thread_exception;
            std::atomic_bool has_exception = false;

            std::jthread jt {[&] (auto token) {
                thread_begin = clock::now();
                try {
                    fn(token, ops_count);
                }
                catch (...) {
                    thread_exception = std::current_exception();
                    has_exception = true;
                }
                thread_end = clock::now();
            }};

            auto begin = clock::now();
            while (!has_exception) {
                auto elapsed = to_sec(clock::now() - begin);
                if (elapsed >= time_)
                    break;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            jt.request_stop();
            jt.join();

            print(thread_end - thread_begin, ops_count);

            if (thread_exception)
                std::rethrow_exception(thread_exception);
        }

    private:
        template <class Duration>
        static double to_sec(const Duration& d) {
            using fsec = std::chrono::duration<double>;
            return std::chrono::duration_cast<fsec>(d).count();
        }

        template <class Duration>
        void print(const Duration& elapsed, uint_fast64_t ops_count) const {
            std::chrono::hh_mm_ss hms(elapsed);
            std::cout << std::setfill(' ') << std::setw(2)
                      << hms.minutes().count() << ":"
                      << std::setfill('0') << std::setw(2)
                      << hms.seconds().count() << "."
                      << std::setfill('0') << std::setw(9)
                      << hms.subseconds().count()
                      << "\t";

            auto [scaled_ops, prefix] = si_prefix(ops_count / to_sec(elapsed));
            std::cout << std::setfill(' ') << std::setw(6)
                      << std::fixed << std::setprecision(2)
                      << scaled_ops << " "
                      << std::setfill(' ') << std::setw(1)
                      << prefix << " op/s"
                      << "\t";

            std::cout << name_ << std::endl << std::flush;
        }

        template <std::floating_point Float>
        static std::tuple<Float, std::string> si_prefix(Float value) {
            auto scaled_value = value;
            for (auto prefix : prefixes_without_last) {
                if (scaled_value + 0.5 < 1000)
                    return {scaled_value, prefix};

                scaled_value /= 1000;
            }
            return {scaled_value, prefixes_last};
        }

        static constexpr const char* prefixes_array[] = {
            "", "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q",
        };
        static constexpr auto prefixes = std::span(prefixes_array);
        static constexpr auto prefixes_without_last = prefixes.first(prefixes.size() - 1);
        static constexpr auto prefixes_last = prefixes.back();
    };

    template <class UuidImpl>
    class uuid_perf_test
    {
        using uuid_t = UuidImpl::uuid_type;
        using array_t = UuidImpl::array_type;

        UuidImpl impl;

        static constexpr double measure_time_short = 3;
        static constexpr double measure_time_long = 10;

        void test_parse() {
            std::vector<std::string> in;
            for (int i = 0; i < 1'000'000; i++)
                in.push_back(impl.to_string(impl.gen_v4_mt()));

            std::vector<uuid_t> out{in.size()};

            ops_measure ops{"parse", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        out[i] = impl.parse(in[i]);

                    ops_count += in.size();
                }
            });
        }

        void test_to_string() {
            std::vector<uuid_t> in;
            for (int i = 0; i < 1'000'000; i++)
                in.push_back(impl.gen_v4_mt());

            std::vector<std::string> out{in.size()};

            ops_measure ops{"to string", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        out[i] = impl.to_string(in[i]);

                    ops_count += in.size();
                }
            });
        }

        void test_to_string_array() {
            std::vector<uuid_t> in;
            for (int i = 0; i < 1'000'000; i++)
                in.push_back(impl.gen_v4_mt());

            std::vector<std::array<char, 40>> out{in.size()};

            ops_measure ops{"to string (array)", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        impl.to_string(in[i], out[i]);

                    ops_count += in.size();
                }
            });
        }

        void test_load_bytes() {
            std::vector<array_t> in;
            for (int i = 0; i < 1'000'000; i++) {
                array_t a;
                impl.to_bytes(impl.gen_v4_mt(), a);
                in.push_back(a);
            }

            std::vector<uuid_t> out{in.size()};

            ops_measure ops{"load bytes", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        out[i] = impl.load_bytes(in[i]);

                    ops_count += in.size();
                }
            });
        }

        void test_to_bytes() {
            std::vector<uuid_t> in;
            for (int i = 0; i < 1'000'000; i++)
                in.push_back(impl.gen_v4_mt());

            std::vector<array_t> out{in.size()};

            ops_measure ops{"to bytes", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (size_t i = 0; i < in.size(); i++)
                        impl.to_bytes(in[i], out[i]);

                    ops_count += in.size();
                }
            });
        }

        void test_compare() {
            std::vector<uuid_t> in;
            for (int i = 0; i < 1'000'000; i++)
                in.push_back(impl.gen_v4_mt());

            auto lhs = impl.gen_v4_mt();
            uint_fast64_t cmp_count = 0;

            ops_measure ops{"compare", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (auto& rhs : in) {
                        if (lhs < rhs)
                            cmp_count++;
                    }
                    ops_count += in.size();
                }
            });

            if (cmp_count == 0)
                throw std::runtime_error("Compare ops_count error");
        }

        void test_generate_v4_mt19937() {
            std::vector<uuid_t> out{1'000'000};

            ops_measure ops{"generate v4 (mt19937)", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (auto& u : out)
                        u = impl.gen_v4_mt();

                    ops_count += out.size();
                }
            });
        }

        void test_generate_v7_mt19937() {
            std::vector<uuid_t> out{1'000'000};

            ops_measure ops{"generate v7 (mt19937)", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (auto& u : out)
                        u = impl.gen_v7_mt();

                    ops_count += out.size();
                }
            });
        }

        void test_generate_v4() {
            std::vector<uuid_t> out{100'000};

            ops_measure ops{"generate v4 (default)", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (auto& u : out)
                        u = impl.gen_v4();

                    ops_count += out.size();
                }
            });
        }

        void test_generate_v7() {
            std::vector<uuid_t> out{100'000};

            ops_measure ops{"generate v7 (default)", measure_time_short};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (auto& u : out)
                        u = impl.gen_v7();

                    ops_count += out.size();
                }
            });
        }

        void test_generate_v4_set() {
            std::set<uuid_t> set;
            constexpr int iteration = 100'000;

            ops_measure ops{"generate v4 (default, std::set)", measure_time_long};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (int i = 0; i < iteration; i++) {
                        auto [it, ok] = set.insert(impl.gen_v4());
                        if (!ok)
                            throw std::runtime_error("UUID collision detected. Please check the random number generation.");
                    }

                    ops_count += iteration;
                }
            });
        }

        void test_generate_v7_set() {
            std::set<uuid_t> set;
            constexpr int iteration = 100'000;

            ops_measure ops{"generate v7 (default, std::set)", measure_time_long};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (int i = 0; i < iteration; i++) {
                        auto [it, ok] = set.insert(impl.gen_v7());
                        if (!ok)
                            throw std::runtime_error("UUID collision detected. Please check the random number generation.");
                    }

                    ops_count += iteration;
                }
            });
        }

        void test_generate_v4_unordered_set() {
            std::unordered_set<uuid_t> set;
            constexpr int iteration = 100'000;

            ops_measure ops{"generate v4 (default, std::unordered_set)", measure_time_long};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (int i = 0; i < iteration; i++) {
                        auto [it, ok] = set.insert(impl.gen_v4());
                        if (!ok)
                            throw std::runtime_error("UUID collision detected. Please check the random number generation.");
                    }

                    ops_count += iteration;
                }
            });
        }

        void test_generate_v7_unordered_set() {
            std::unordered_set<uuid_t> set;
            constexpr int iteration = 100'000;

            ops_measure ops{"generate v7 (default, std::unordered_set)", measure_time_long};
            ops.measure([&](auto token, auto& ops_count) {
                while (!token.stop_requested()) {
                    for (int i = 0; i < iteration; i++) {
                        auto [it, ok] = set.insert(impl.gen_v7());
                        if (!ok)
                            throw std::runtime_error("UUID collision detected. Please check the random number generation.");
                    }

                    ops_count += iteration;
                }
            });
        }

        using test_fn = void(uuid_perf_test::*)();

        static constexpr test_fn tests[] = {
            &uuid_perf_test::test_parse,
            &uuid_perf_test::test_to_string,
            &uuid_perf_test::test_to_string_array,
            &uuid_perf_test::test_load_bytes,
            &uuid_perf_test::test_to_bytes,
            &uuid_perf_test::test_compare,
            &uuid_perf_test::test_generate_v4_mt19937,
            &uuid_perf_test::test_generate_v7_mt19937,
            &uuid_perf_test::test_generate_v4,
            &uuid_perf_test::test_generate_v7,
            &uuid_perf_test::test_generate_v4_set,
            &uuid_perf_test::test_generate_v7_set,
            &uuid_perf_test::test_generate_v4_unordered_set,
            &uuid_perf_test::test_generate_v7_unordered_set,
        };

    public:
        void run() {
            for (auto test : tests) {
                try {
                    (this->*test)();
                }
                catch (not_implemented&) {}
            }
        }
    };
}
