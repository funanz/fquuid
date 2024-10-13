// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstdint>
#include <span>
#include <stdexcept>
#include "fquuid.hpp"

namespace fquuid
{
    class uuid_binary
    {
        static constexpr uint64_t load_u64(std::span<const uint8_t> bytes) {
            return (static_cast<uint64_t>(bytes[0]) << 56 |
                    static_cast<uint64_t>(bytes[1]) << 48 |
                    static_cast<uint64_t>(bytes[2]) << 40 |
                    static_cast<uint64_t>(bytes[3]) << 32 |
                    static_cast<uint64_t>(bytes[4]) << 24 |
                    static_cast<uint64_t>(bytes[5]) << 16 |
                    static_cast<uint64_t>(bytes[6]) << 8 |
                    static_cast<uint64_t>(bytes[7]));
        }

        static constexpr void store_u64(uint64_t x, std::span<uint8_t> bytes) {
            bytes[0] = static_cast<uint8_t>(x >> 56);
            bytes[1] = static_cast<uint8_t>(x >> 48);
            bytes[2] = static_cast<uint8_t>(x >> 40);
            bytes[3] = static_cast<uint8_t>(x >> 32);
            bytes[4] = static_cast<uint8_t>(x >> 24);
            bytes[5] = static_cast<uint8_t>(x >> 16);
            bytes[6] = static_cast<uint8_t>(x >> 8);
            bytes[7] = static_cast<uint8_t>(x);
        }

    public:
        static constexpr void load_from_bytes(uuid_u64& u, std::span<const uint8_t> bytes) {
            if (bytes.size() < 16)
                throw std::invalid_argument("uuid::load_from_bytes() input span size is small");

            u[0] = load_u64(bytes.subspan(0, 8));
            u[1] = load_u64(bytes.subspan(8, 8));
        }

        static constexpr void store_to_bytes(const uuid_u64& u, std::span<uint8_t> bytes) {
            if (bytes.size() < 16)
                throw std::invalid_argument("uuid::store_to_bytes() output span size is small");

            store_u64(u[0], bytes.subspan(0, 8));
            store_u64(u[1], bytes.subspan(8, 8));
        }
    };
}
