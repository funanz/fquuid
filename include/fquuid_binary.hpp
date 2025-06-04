// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include "fquuid_types.hpp"
#include "fquuid_spanner.hpp"

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

        static constexpr uint64_t load_u64(std::span<const ByteT, 8> bytes) {
            return (to_u64(fixed_at<0>(bytes)) << 56 |
                    to_u64(fixed_at<1>(bytes)) << 48 |
                    to_u64(fixed_at<2>(bytes)) << 40 |
                    to_u64(fixed_at<3>(bytes)) << 32 |
                    to_u64(fixed_at<4>(bytes)) << 24 |
                    to_u64(fixed_at<5>(bytes)) << 16 |
                    to_u64(fixed_at<6>(bytes)) << 8 |
                    to_u64(fixed_at<7>(bytes)));
        }

        static constexpr void store_u64(uint64_t x, std::span<ByteT, 8> bytes) {
            fixed_at<0>(bytes) = to_byte(x >> 56);
            fixed_at<1>(bytes) = to_byte(x >> 48);
            fixed_at<2>(bytes) = to_byte(x >> 40);
            fixed_at<3>(bytes) = to_byte(x >> 32);
            fixed_at<4>(bytes) = to_byte(x >> 24);
            fixed_at<5>(bytes) = to_byte(x >> 16);
            fixed_at<6>(bytes) = to_byte(x >> 8);
            fixed_at<7>(bytes) = to_byte(x);
        }

    public:
        static constexpr void load_from_bytes(uuid_u64& u, std::span<const ByteT> bytes) {
            if (auto fixed = try_fixed<16>(bytes); fixed) {
                u[0] = load_u64(fixed_subspan<0, 8>(*fixed));
                u[1] = load_u64(fixed_subspan<8, 8>(*fixed));
            } else {
                throw std::invalid_argument("uuid:load_from_bytes: input span size is small");
            }
        }

        static constexpr void store_to_bytes(const uuid_u64& u, std::span<ByteT> bytes) {
            if (auto fixed = try_fixed<16>(bytes); fixed) {
                store_u64(u[0], fixed_subspan<0, 8>(*fixed));
                store_u64(u[1], fixed_subspan<8, 8>(*fixed));
            } else {
                throw std::invalid_argument("uuid:store_to_bytes: output span size is small");
            }
        }
    };

    using uuid_binary_u8 = uuid_basic_binary<uint8_t>;
    using uuid_binary_byte = uuid_basic_binary<std::byte>;
}
