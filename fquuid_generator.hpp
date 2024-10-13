#pragma once
#include "fquuid.hpp"
#include "fquuid_random.hpp"

namespace fquuid
{
    class uuid_generator
    {
        using uuid_array = std::array<uint64_t, 2>;

        uuid_random default_rng;

        static void set_version(uuid_array& u, uint64_t ver) {
            u[0] = (u[0] & 0xffff'ffff'ffff'0fff) | ((ver & 0x0f) << 12);
        }

        static void set_variant(uuid_array& u, uint64_t var) {
            u[1] = (u[1] & 0x3fff'ffff'ffff'ffff) | ((var & 0x03) << 62);
        }

        static void set_unix_ts_ms(uuid_array& u, int64_t ms) {
            u[0] = (u[0] & 0x0000'0000'0000'ffff) | (ms << 16);
        }

        static int64_t get_utc_now() {
            auto tp = std::chrono::system_clock::now();
            auto d = tp.time_since_epoch();
            auto dms = std::chrono::duration_cast<std::chrono::milliseconds>(d);
            return dms.count();
        }

    public:
        uuid v4() {
            return v4(default_rng);
        }

        uuid v7() {
            return v7(default_rng);
        }

        template <class RNG>
        uuid v4(RNG& rng) {
            std::uniform_int_distribution<uint64_t> dist64;
            uuid_array u;
            u[0] = dist64(rng);
            u[1] = dist64(rng);

            set_version(u, 4);
            set_variant(u, 0b10);

            return uuid{u};
        }

        template <class RNG>
        uuid v7(RNG& rng) {
            return v7(rng, get_utc_now());
        }

        template <class RNG>
        uuid v7(RNG& rng, int64_t ms) {
            std::uniform_int_distribution<uint16_t> dist16;
            std::uniform_int_distribution<uint64_t> dist64;
            uuid_array u;
            u[0] = dist16(rng);
            u[1] = dist64(rng);

            set_unix_ts_ms(u, ms);
            set_version(u, 7);
            set_variant(u, 0b10);

            return uuid{u};
        }
    };
}
