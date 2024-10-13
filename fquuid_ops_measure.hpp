#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <concepts>
#include <cstdint>
#include <functional>
#include <iostream>
#include <span>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <thread>
#include <tuple>

namespace fquuid
{
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

            if (thread_exception) {
                print_blank();
                std::rethrow_exception(thread_exception);
            } else {
                print(thread_end - thread_begin, ops_count);
            }
        }

    private:
        template <class Duration>
        static double to_sec(const Duration& d) {
            using fsec = std::chrono::duration<double>;
            return std::chrono::duration_cast<fsec>(d).count();
        }

        void print_blank() const {
            std::cout << std::setw(15) << ' ' << "\t"
                      << std::setw(13) << ' ' << "\t"
                      << name_ << std::endl << std::flush;
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
}
