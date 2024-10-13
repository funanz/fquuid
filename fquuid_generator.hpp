// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstdint>
#include <random>
#include "fquuid_uuid.hpp"
#include "fquuid_random.hpp"
#include "fquuid_clock.hpp"

namespace fquuid
{
    class uuid_generator
    {
        uuid_random rng_;

        static void set_version(uuid_u64& u, uint64_t ver) {
            u[0] = (u[0] & 0xffff'ffff'ffff'0fff) | ((ver & 0x0f) << 12);
        }

        static void set_variant(uuid_u64& u, uint64_t var) {
            u[1] = (u[1] & 0x3fff'ffff'ffff'ffff) | ((var & 0x03) << 62);
        }

        static void set_unix_ts_ms(uuid_u64& u, int64_t ms) {
            u[0] = (u[0] & 0x0000'0000'0000'ffff) | (ms << 16);
        }

    public:
        uuid v4() {
            return v4(rng_);
        }

        uuid v7() {
            return v7(rng_);
        }

        template <class RNG>
        static uuid v4(RNG& rng) {
            std::uniform_int_distribution<uint64_t> dist64;
            uuid_u64 u;
            u[0] = dist64(rng);
            u[1] = dist64(rng);

            set_version(u, 4);
            set_variant(u, 0b10);

            return uuid{u};
        }

        template <class RNG>
        static uuid v7(RNG& rng) {
            return v7(rng, uuid_clock::now());
        }

        template <class RNG>
        static uuid v7(RNG& rng, int64_t ms) {
            std::uniform_int_distribution<uint16_t> dist16;
            std::uniform_int_distribution<uint64_t> dist64;
            uuid_u64 u;
            u[0] = dist16(rng);
            u[1] = dist64(rng);

            set_unix_ts_ms(u, ms);
            set_version(u, 7);
            set_variant(u, 0b10);

            return uuid{u};
        }
    };
}
