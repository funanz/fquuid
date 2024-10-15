// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include "fquuid.hpp"

namespace fquuid
{
    class uuid_string
    {
        static constexpr auto hex_to_u4_table = [] {
            std::array<int8_t, 256> a;
            for (size_t i = 0; i < a.size(); i++) {
                if ('0' <= i && i <= '9')
                    a[i] =  i - '0';
                else if ('A' <= i && i <= 'F')
                    a[i] =  i - 'A' + 10;
                else if ('a' <= i && i <= 'f')
                    a[i] =  i - 'a' + 10;
                else
                    a[i] = -1;
            }
            return a;
        }();

        static constexpr uint64_t hex_to_u4(char c) {
            static_assert(hex_to_u4_table.size() == 256);

            return hex_to_u4_table[static_cast<size_t>(c) & 0xff];
        }

        static constexpr uint64_t hex4_to_u16(std::span<const char> s) {
            auto u16e = (hex_to_u4(s[0]) << 12 |
                         hex_to_u4(s[1]) << 8 |
                         hex_to_u4(s[2]) << 4 |
                         hex_to_u4(s[3]) << 0);

            if (u16e > 0xffff)
                throw std::invalid_argument("invalid hex character [0-9a-fA-F]");
            else
                return u16e;
        }

        template <int Bit>
        requires (Bit == 64 || Bit == 48 || Bit == 32 ||Bit == 16)
        static constexpr uint64_t load_hex(std::span<const char> s) {
            constexpr size_t Adjust = 16 - Bit / 4;
            uint64_t x = 0;

            if constexpr (Bit == 64)
                x |= hex4_to_u16(s.subspan(0 - Adjust, 4)) << 48;
            if constexpr (Bit >= 48)
                x |= hex4_to_u16(s.subspan(4 - Adjust, 4)) << 32;
            if constexpr (Bit >= 32)
                x |= hex4_to_u16(s.subspan(8 - Adjust, 4)) << 16;
            if constexpr (Bit >= 16)
                x |= hex4_to_u16(s.subspan(12 - Adjust, 4));

            return x;
        }

        static constexpr char u4_to_hex(uint64_t u4) {
            constexpr auto u4_to_hex_table = "0123456789abcdef";

            return u4_to_hex_table[u4 & 0xf];
        }

        static constexpr void u16_to_hex4(uint64_t u16, std::span<char> s) {
            s[0] = u4_to_hex(u16 >> 12);
            s[1] = u4_to_hex(u16 >> 8);
            s[2] = u4_to_hex(u16 >> 4);
            s[3] = u4_to_hex(u16);
        }

        template <int Bit>
        requires (Bit == 64 || Bit == 48 || Bit == 32 ||Bit == 16)
        static constexpr void store_hex(uint64_t x, std::span<char> s) {
            constexpr size_t Adjust = 16 - Bit / 4;

            if constexpr (Bit == 64)
                u16_to_hex4(x >> 48, s.subspan(0 - Adjust, 4));
            if constexpr (Bit >= 48)
                u16_to_hex4(x >> 32, s.subspan(4 - Adjust, 4));
            if constexpr (Bit >= 32)
                u16_to_hex4(x >> 16, s.subspan(8 - Adjust, 4));
            if constexpr (Bit >= 16)
                u16_to_hex4(x, s.subspan(12 - Adjust, 4));
        }

        static constexpr bool is_standerd_format(std::span<const char> s) {
            if (s.size() < 36) return false;

            return (s[ 8] == '-' && s[13] == '-' &&
                    s[18] == '-' && s[23] == '-');
        }

    public:
        static constexpr void parse(uuid_u64& u, std::span<const char> s) {
            if (is_standerd_format(s)) {
                // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
                u[0] = (load_hex<32>(s.subspan(0, 8)) << 32 |
                        load_hex<16>(s.subspan(9, 4)) << 16 |
                        load_hex<16>(s.subspan(14, 4)));
                u[1] = (load_hex<16>(s.subspan(19, 4)) << 48 |
                        load_hex<48>(s.subspan(24, 12)));
            }
            else if (s.size() >= 32) {
                // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                u[0] = load_hex<64>(s.subspan(0, 16));
                u[1] = load_hex<64>(s.subspan(16, 16));
            }
            else {
                throw std::invalid_argument("uuid::parse() input span size is small");
            }
        }

        static constexpr void to_string(const uuid_u64& u, std::span<char> s, bool null_terminate) {
            size_t size = null_terminate ? 37 : 36;
            if (s.size() < size)
                throw std::invalid_argument("uuid::to_string() output span size is small");

            store_hex<32>(u[0] >> 32, s.subspan(0, 8));
            store_hex<16>(u[0] >> 16, s.subspan(9, 4));
            store_hex<16>(u[0],       s.subspan(14, 4));
            store_hex<16>(u[1] >> 48, s.subspan(19, 4));
            store_hex<48>(u[1],       s.subspan(24, 12));

            s[8] = s[13] = s[18] = s[23] = '-';

            if (null_terminate)
                s[36] = 0;
        }

        static constexpr size_t strlen(const char* s) {
            auto p = s;
            while (*p)
                p++;
            return p - s;
        }
    };
}
