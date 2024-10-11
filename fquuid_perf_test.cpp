#include <array>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <vector>
#include "fquuid.hpp"

constexpr double measure_time_short = 3;
constexpr double measure_time_long = 10;

class ops_measure
{
    std::string name_;
    double time_;

public:
    using measure_fn = std::function<void(std::stop_token, uint_fast64_t&)>;

    ops_measure(const std::string& name, double time)
        : name_(name), time_(time) {}

    void measure(measure_fn fn) const {
        using clock = std::chrono::high_resolution_clock;

        uint_fast64_t count = 0;
        std::jthread jt{fn, std::ref(count)};
        auto begin = clock::now();

        for (;;) {
            auto sec = to_sec(clock::now() - begin);
            if (sec >= time_)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        jt.request_stop();
        jt.join();
        auto end = clock::now();

        print(end - begin, count);
    }

private:
    template <class Duration>
    static double to_sec(const Duration& d) {
        using fsec = std::chrono::duration<double>;
        return std::chrono::duration_cast<fsec>(d).count();
    }

    template <class Duration>
    void print(const Duration& elapsed, uint_fast64_t count) const {
        std::chrono::hh_mm_ss hms(elapsed);
        std::cout << std::setfill(' ') << std::setw(2)
                  << hms.minutes().count()
                  << ":"
                  << std::setfill('0') << std::setw(2)
                  << hms.seconds().count()
                  << "."
                  << std::setfill('0') << std::setw(9)
                  << hms.subseconds().count()
                  << "\t";

        auto sec = to_sec(elapsed);
        auto [scaled_count_sec, prefix] = si_prefix(count / sec);
        std::cout << std::setfill(' ') << std::setw(6)
                  << std::fixed << std::setprecision(2)
                  << scaled_count_sec << " " << prefix << " op/s"
                  << "\t";

        std::cout << name_ << std::endl << std::flush;
    }

    template <class Float>
    requires (std::is_floating_point_v<Float>)
    static std::tuple<Float, std::string> si_prefix(Float value) {
        static constexpr const char* prefixes_array[] = {
            "", "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q",
        };
        static constexpr auto prefixes = std::span(prefixes_array);
        static constexpr auto prefixes_without_last = prefixes.first(prefixes.size() - 1);
        static constexpr auto prefixes_last = prefixes.back();

        auto scaled_value = value;
        for (auto prefix : prefixes_without_last) {
            if (scaled_value + 0.5 < 1000)
                return {scaled_value, prefix};

            scaled_value /= 1000;
        }

        return {scaled_value, prefixes_last};
    }
};

static void test_parse()
{
    std::mt19937 rng; // [INSECURE] for performance test

    std::vector<std::string> in;
    for (int i = 0; i < 1'000'000; i++)
        in.push_back(fquuid::uuid::v4(rng).to_string());

    std::vector<fquuid::uuid> out{in.size()};

    ops_measure ops{"parse", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (size_t i = 0; i < in.size(); i++)
                out[i] = fquuid::uuid{in[i]};

            count += in.size();
        }
    });
}

static void test_to_string()
{
    std::mt19937 rng; // [INSECURE] for performance test

    std::vector<fquuid::uuid> in;
    for (int i = 0; i < 1'000'000; i++)
        in.push_back(fquuid::uuid::v4(rng));

    std::vector<std::string> out{in.size()};

    ops_measure ops{"to string", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (size_t i = 0; i < in.size(); i++)
                out[i] = in[i].to_string();

            count += in.size();
        }
    });
}

static void test_load_bytes()
{
    std::mt19937 rng; // [INSECURE] for performance test

    std::vector<std::array<uint8_t, 16>> in;
    for (int i = 0; i < 1'000'000; i++) {
        std::array<uint8_t, 16> a;
        fquuid::uuid::v4(rng).to_bytes(a);
        in.push_back(a);
    }

    std::vector<fquuid::uuid> out{in.size()};

    ops_measure ops{"load bytes", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (size_t i = 0; i < in.size(); i++)
                out[i] = fquuid::uuid{in[i]};

            count += in.size();
        }
    });
}

static void test_to_bytes()
{
    std::mt19937 rng; // [INSECURE] for performance test

    std::vector<fquuid::uuid> in;
    for (int i = 0; i < 1'000'000; i++)
        in.push_back(fquuid::uuid::v4(rng));

    std::vector<std::array<uint8_t, 16>> out{in.size()};

    ops_measure ops{"to bytes", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (size_t i = 0; i < in.size(); i++)
                in[i].to_bytes(out[i]);

            count += in.size();
        }
    });
}

static void test_compare()
{
    std::mt19937 rng; // [INSECURE] for performance test

    std::vector<fquuid::uuid> in;
    for (int i = 0; i < 1'000'000; i++)
        in.push_back(fquuid::uuid::v4(rng));

    auto lhs = fquuid::uuid::v4(rng);
    uint_fast64_t count_less = 0;

    ops_measure ops{"compare", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (auto& rhs : in) {
                if (lhs < rhs)
                    count_less++;
            }
            count += in.size();
        }
    });

    if (count_less == 0)
        throw std::runtime_error("test_compare() dead-code elimination");
}

static void test_generate_v4_mt19937()
{
    std::mt19937 rng; // [INSECURE] for performance test
    std::vector<fquuid::uuid> out{1'000'000};

    ops_measure ops{"generate v4 (mt19937)", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (auto& u : out)
                u = fquuid::uuid::v4(rng);

            count += out.size();
        }
    });
}

static void test_generate_v7_mt19937()
{
    std::mt19937 rng; // [INSECURE] for performance test
    std::vector<fquuid::uuid> out{1'000'000};

    ops_measure ops{"generate v7 (mt19937)", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (auto& u : out)
                u = fquuid::uuid::v7(rng);

            count += out.size();
        }
    });
}

static void test_generate_v4()
{
    std::vector<fquuid::uuid> out{100'000};

    ops_measure ops{"generate v4 (default)", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (auto& u : out)
                u = fquuid::uuid::v4();

            count += out.size();
        }
    });
}

static void test_generate_v7()
{
    std::vector<fquuid::uuid> out{100'000};

    ops_measure ops{"generate v7 (default)", measure_time_short};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (auto& u : out)
                u = fquuid::uuid::v7();

            count += out.size();
        }
    });
}

static void test_generate_v4_set()
{
    std::set<fquuid::uuid> set;
    constexpr int iteration = 100'000;

    ops_measure ops{"generate v4 (default, std::set)", measure_time_long};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (int i = 0; i < iteration; i++)
                set.insert(fquuid::uuid::v4());

            count += iteration;
        }
    });
}

static void test_generate_v7_set()
{
    std::set<fquuid::uuid> set;
    constexpr int iteration = 100'000;

    ops_measure ops{"generate v7 (default, std::set)", measure_time_long};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (int i = 0; i < iteration; i++)
                set.insert(fquuid::uuid::v7());

            count += iteration;
        }
    });
}

static void test_generate_v4_unordered_set()
{
    std::unordered_set<fquuid::uuid> set;
    constexpr int iteration = 100'000;

    ops_measure ops{"generate v4 (default, std::unordered_set)", measure_time_long};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (int i = 0; i < iteration; i++)
                set.insert(fquuid::uuid::v4());

            count += iteration;
        }
    });
}

static void test_generate_v7_unordered_set()
{
    std::unordered_set<fquuid::uuid> set;
    constexpr int iteration = 100'000;

    ops_measure ops{"generate v7 (default, std::unordered_set)", measure_time_long};
    ops.measure([&](auto token, auto& count) {
        while (!token.stop_requested()) {
            for (int i = 0; i < iteration; i++)
                set.insert(fquuid::uuid::v7());

            count += iteration;
        }
    });
}

int main(int argc, char** argv)
{
    test_parse();
    test_to_string();
    test_load_bytes();
    test_to_bytes();
    test_compare();
    test_generate_v4_mt19937();
    test_generate_v7_mt19937();
    test_generate_v4();
    test_generate_v7();
    test_generate_v4_set();
    test_generate_v7_set();
    test_generate_v4_unordered_set();
    test_generate_v7_unordered_set();
}
