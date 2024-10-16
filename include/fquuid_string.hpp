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
    template <class CharT>
    class uuid_basic_string
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

        // ok = 0x00 - 0x0f, error = 0xffff'ffff'ffff'ffff
        static constexpr uint64_t hex_to_u4(CharT c) {
            if constexpr (sizeof(c) > 1) {
                if (static_cast<size_t>(c) > 0xff)
                    return -1;
            }

            static_assert(hex_to_u4_table.size() == 256);
            return hex_to_u4_table[static_cast<size_t>(c) & 0xff];
        }

        static constexpr uint64_t load_u16_hex(std::span<const CharT> s) {
            auto u16e = (hex_to_u4(s[0]) << 12 |
                         hex_to_u4(s[1]) << 8 |
                         hex_to_u4(s[2]) << 4 |
                         hex_to_u4(s[3]));

            if (u16e >> 16)
                throw std::invalid_argument("invalid hex character [0-9a-fA-F]");
            else
                return u16e;
        }

        static constexpr uint64_t load_u32_hex(std::span<const CharT> s) {
            auto u32e = (hex_to_u4(s[0]) << 28 |
                         hex_to_u4(s[1]) << 24 |
                         hex_to_u4(s[2]) << 20 |
                         hex_to_u4(s[3]) << 16 |
                         hex_to_u4(s[4]) << 12 |
                         hex_to_u4(s[5]) << 8 |
                         hex_to_u4(s[6]) << 4 |
                         hex_to_u4(s[7]));

            if (u32e >> 32)
                throw std::invalid_argument("invalid hex character [0-9a-fA-F]");
            else
                return u32e;
        }

        static constexpr uint64_t load_u48_hex(std::span<const CharT> s) {
            auto u48e = (hex_to_u4(s[0]) << 44 |
                         hex_to_u4(s[1]) << 40 |
                         hex_to_u4(s[2]) << 36 |
                         hex_to_u4(s[3]) << 32 |
                         hex_to_u4(s[4]) << 28 |
                         hex_to_u4(s[5]) << 24 |
                         hex_to_u4(s[6]) << 20 |
                         hex_to_u4(s[7]) << 16 |
                         hex_to_u4(s[8]) << 12 |
                         hex_to_u4(s[9]) << 8 |
                         hex_to_u4(s[10]) << 4 |
                         hex_to_u4(s[11]));

            if (u48e >> 48)
                throw std::invalid_argument("invalid hex character [0-9a-fA-F]");
            else
                return u48e;
        }

        static constexpr uint64_t load_u64_hex(std::span<const CharT> s) {
            return (load_u32_hex(s.subspan(0, 8)) << 32 |
                    load_u32_hex(s.subspan(8, 8)));
        }

        static constexpr auto u4_to_hex_table = [] {
            std::array<CharT, 16> a;
            for (int i = 0; i < 10; i++)
                a[i] = '0' + i;
            for (int i = 0; i < 6; i++)
                a[i + 10] = 'a' + i;
            return a;
        }();

        static constexpr CharT u4_to_hex(uint64_t u4) {
            static_assert(u4_to_hex_table.size() == 16);

            return u4_to_hex_table[u4 & 0xf];
        }

        static constexpr void store_u16_hex(uint64_t u16, std::span<CharT> s) {
            s[0] = u4_to_hex(u16 >> 12);
            s[1] = u4_to_hex(u16 >> 8);
            s[2] = u4_to_hex(u16 >> 4);
            s[3] = u4_to_hex(u16);
        }

        static constexpr void store_u32_hex(uint64_t x, std::span<CharT> s) {
            store_u16_hex(x >> 16, s.subspan(0, 4));
            store_u16_hex(x, s.subspan(4, 4));
        }

        static constexpr void store_u48_hex(uint64_t x, std::span<CharT> s) {
            store_u16_hex(x >> 32, s.subspan(0, 4));
            store_u16_hex(x >> 16, s.subspan(4, 4));
            store_u16_hex(x, s.subspan(8, 4));
        }

        static constexpr bool is_standerd_format(std::span<const CharT> s) {
            if (s.size() < 36) return false;

            return (s[ 8] == '-' && s[13] == '-' &&
                    s[18] == '-' && s[23] == '-');
        }

    public:
        static constexpr void parse(uuid_u64& u, std::span<const CharT> s) {
            if (is_standerd_format(s)) {
                // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
                u[0] = (load_u32_hex(s.subspan(0, 8)) << 32 |
                        load_u16_hex(s.subspan(9, 4)) << 16 |
                        load_u16_hex(s.subspan(14, 4)));
                u[1] = (load_u16_hex(s.subspan(19, 4)) << 48 |
                        load_u48_hex(s.subspan(24, 12)));
            }
            else if (s.size() >= 32) {
                // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                u[0] = load_u64_hex(s.subspan(0, 16));
                u[1] = load_u64_hex(s.subspan(16, 16));
            }
            else {
                throw std::invalid_argument("uuid::parse() input span size is small");
            }
        }

        static constexpr void to_string(const uuid_u64& u, std::span<CharT> s, bool null_terminate) {
            size_t size = null_terminate ? 37 : 36;
            if (s.size() < size)
                throw std::invalid_argument("uuid::to_string() output span size is small");

            store_u32_hex(u[0] >> 32, s.subspan(0, 8));
            store_u16_hex(u[0] >> 16, s.subspan(9, 4));
            store_u16_hex(u[0],       s.subspan(14, 4));
            store_u16_hex(u[1] >> 48, s.subspan(19, 4));
            store_u48_hex(u[1],       s.subspan(24, 12));

            s[8] = s[13] = s[18] = s[23] = '-';

            if (null_terminate)
                s[36] = 0;
        }

        static constexpr size_t strlen(const CharT* s) {
            auto p = s;
            while (*p)
                p++;
            return p - s;
        }
    };

    using uuid_string = uuid_basic_string<char>;
    using uuid_wstring = uuid_basic_string<wchar_t>;
}
