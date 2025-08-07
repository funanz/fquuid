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
    class uuid_generator_v4
    {
        uuid_random rng_;

    public:
        uuid operator ()() {
            return generate(rng_);
        }

        template <class RNG>
        static uuid generate(RNG& rng) {
            std::uniform_int_distribution<uint64_t> dist64;

            detail::uuid_u128 u { dist64(rng), dist64(rng) };
            u.version(4);
            u.variant(0b10);

            return uuid{u};
        }
    };

    class uuid_generator_v7
    {
        uuid_random rng_;

    public:
        uuid operator ()() {
            return generate(rng_);
        }

        template <class RNG>
        static uuid generate(RNG& rng) {
            return generate(rng, detail::uuid_clock::now());
        }

        template <class RNG>
        static uuid generate(RNG& rng, int64_t ms) {
            std::uniform_int_distribution<uint16_t> dist16;
            std::uniform_int_distribution<uint64_t> dist64;

            detail::uuid_u128 u { dist16(rng), dist64(rng) };
            u.unix_ts_ms(ms);
            u.version(7);
            u.variant(0b10);

            return uuid{u};
        }
    };
}
