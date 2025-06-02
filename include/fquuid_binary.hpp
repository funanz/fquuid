// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include "fquuid_types.hpp"

namespace fquuid::detail
{
    template <ByteLike ByteT>
    class uuid_basic_binary
    {
        static constexpr uint64_t to_u64(ByteT b) {
            return static_cast<uint8_t>(b);
        }

        static constexpr ByteT to_byte(uint64_t x) {
            return ByteT(static_cast<uint8_t>(x));
        }

        static constexpr uint64_t load_u64(std::span<const ByteT> bytes) {
            return (to_u64(bytes[0]) << 56 |
                    to_u64(bytes[1]) << 48 |
                    to_u64(bytes[2]) << 40 |
                    to_u64(bytes[3]) << 32 |
                    to_u64(bytes[4]) << 24 |
                    to_u64(bytes[5]) << 16 |
                    to_u64(bytes[6]) << 8 |
                    to_u64(bytes[7]));
        }

        static constexpr void store_u64(uint64_t x, std::span<ByteT> bytes) {
            bytes[0] = to_byte(x >> 56);
            bytes[1] = to_byte(x >> 48);
            bytes[2] = to_byte(x >> 40);
            bytes[3] = to_byte(x >> 32);
            bytes[4] = to_byte(x >> 24);
            bytes[5] = to_byte(x >> 16);
            bytes[6] = to_byte(x >> 8);
            bytes[7] = to_byte(x);
        }

    public:
        static constexpr void load_from_bytes(uuid_u64& u, std::span<const ByteT> bytes) {
            if (bytes.size() < 16)
                throw std::invalid_argument("uuid::load_from_bytes() input span size is small");

            u[0] = load_u64(bytes.subspan(0, 8));
            u[1] = load_u64(bytes.subspan(8, 8));
        }

        static constexpr void store_to_bytes(const uuid_u64& u, std::span<ByteT> bytes) {
            if (bytes.size() < 16)
                throw std::invalid_argument("uuid::store_to_bytes() output span size is small");

            store_u64(u[0], bytes.subspan(0, 8));
            store_u64(u[1], bytes.subspan(8, 8));
        }
    };

    using uuid_binary_u8 = uuid_basic_binary<uint8_t>;
    using uuid_binary_byte = uuid_basic_binary<std::byte>;
}
