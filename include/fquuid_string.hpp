// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include "fquuid_types.hpp"
#include "fquuid_spanner.hpp"

namespace fquuid::detail
{
    template <class CharT>
    class uuid_basic_string
    {
        static constexpr auto hex_to_u4_table = [] {
            std::array<int8_t, 256> a;
            for (size_t i = 0; i < a.size(); i++) {
                if (0x30 <= i && i <= 0x39) // [0-9]
                    a[i] =  i - 0x30;
                else if (0x41 <= i && i <= 0x46) // [A-F]
                    a[i] =  i - 0x41 + 10;
                else if (0x61 <= i && i <= 0x66) // [a-f]
                    a[i] =  i - 0x61 + 10;
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

        static constexpr uint64_t load_u16_hex(std::span<const CharT, 4> s) {
            auto u16e = (hex_to_u4(s[0]) << 12 |
                         hex_to_u4(s[1]) << 8 |
                         hex_to_u4(s[2]) << 4 |
                         hex_to_u4(s[3]));

            if (u16e >> 16)
                throw std::invalid_argument("invalid hex character [0-9a-fA-F]");
            else
                return u16e;
        }

        static constexpr uint64_t load_u32_hex(std::span<const CharT, 8> s) {
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

        static constexpr uint64_t load_u48_hex(std::span<const CharT, 12> s) {
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

        static constexpr uint64_t load_u64_hex(std::span<const CharT, 16> s) {
            return (load_u32_hex(fixed_subspan<0, 8>(s)) << 32 |
                    load_u32_hex(fixed_subspan<8, 8>(s)));
        }

        static constexpr auto u4_to_hex_table = [] {
            std::array<CharT, 16> a;
            for (int i = 0; i < 10; i++)
                a[i] = 0x30 + i; // [0-9]
            for (int i = 0; i < 6; i++)
                a[10 + i] = 0x61 + i; // [a-f]
            return a;
        }();

        static constexpr CharT u4_to_hex(uint64_t u4) {
            static_assert(u4_to_hex_table.size() == 16);

            return u4_to_hex_table[u4 & 0xf];
        }

        static constexpr void store_u16_hex(uint64_t u16, std::span<CharT, 4> s) {
            s[0] = u4_to_hex(u16 >> 12);
            s[1] = u4_to_hex(u16 >> 8);
            s[2] = u4_to_hex(u16 >> 4);
            s[3] = u4_to_hex(u16);
        }

        static constexpr void store_u32_hex(uint64_t x, std::span<CharT, 8> s) {
            store_u16_hex(x >> 16, fixed_subspan<0, 4>(s));
            store_u16_hex(x, fixed_subspan<4, 4>(s));
        }

        static constexpr void store_u48_hex(uint64_t x, std::span<CharT, 12> s) {
            store_u16_hex(x >> 32, fixed_subspan<0, 4>(s));
            store_u16_hex(x >> 16, fixed_subspan<4, 4>(s));
            store_u16_hex(x, fixed_subspan<8, 4>(s));
        }

        static constexpr std::span<const CharT> trim_null_terminator(std::span<const CharT> s) {
            if (s.size() >= 1 && s.back() == 0)
                return s.first(s.size() - 1);
            return s;
        }

        static constexpr std::span<const CharT> trim_braces(std::span<const CharT> s) {
            if (s.size() >= 2 && s.front() == '{' && s.back() == '}')
                return s.subspan(1, s.size() - 2);
            return s;
        }

        static constexpr bool has_dashes(std::span<const CharT, 36> s) {
            return (s[ 8] == '-' && s[13] == '-' &&
                    s[18] == '-' && s[23] == '-');
        }

        // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        static constexpr void parse_standard_format(uuid_u64& u, std::span<const CharT, 36> s) {
            if (!has_dashes(s))
                throw std::invalid_argument("uuid::parse() invalid UUID format");

            u[0] = (load_u32_hex(fixed_subspan<0, 8>(s)) << 32 |
                    load_u16_hex(fixed_subspan<9, 4>(s)) << 16 |
                    load_u16_hex(fixed_subspan<14, 4>(s)));
            u[1] = (load_u16_hex(fixed_subspan<19, 4>(s))) << 48 |
                    load_u48_hex(fixed_subspan<24, 12>(s));
        }

        // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        static constexpr void parse_hex_format(uuid_u64& u, std::span<const CharT, 32> s) {
            u[0] = load_u64_hex(fixed_subspan<0, 16>(s));
            u[1] = load_u64_hex(fixed_subspan<16, 16>(s));
        }

        // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        static constexpr void write_standard_format(const uuid_u64& u, std::span<CharT, 36> s) {
            store_u32_hex(u[0] >> 32, fixed_subspan<0, 8>(s));
            store_u16_hex(u[0] >> 16, fixed_subspan<9, 4>(s));
            store_u16_hex(u[0],       fixed_subspan<14, 4>(s));
            store_u16_hex(u[1] >> 48, fixed_subspan<19, 4>(s));
            store_u48_hex(u[1],       fixed_subspan<24, 12>(s));

            s[8] = s[13] = s[18] = s[23] = '-';
        }

    public:
        static constexpr void parse(uuid_u64& u, std::span<const CharT> s) {
            auto trimmed = trim_braces(trim_null_terminator(s));

            if (auto fixed = try_fixed_equal<36>(trimmed); fixed)
                parse_standard_format(u, *fixed);
            else if (auto fixed = try_fixed_equal<32>(trimmed); fixed)
                parse_hex_format(u, *fixed);
            else
                throw std::invalid_argument("uuid:parse: invalid UUID string length");
        }

        static constexpr void parse(uuid_u64& u, const CharT* s) {
            if (s == nullptr)
                throw std::invalid_argument("uuid:parse: argument is nullptr");

            parse(u, std::basic_string_view<CharT>(s));
        }

        static constexpr void write(const uuid_u64& u, std::span<CharT> s, string_terminator term) {
            if (term == string_terminator::null) {
                if (auto fixed = try_fixed<37>(s); fixed) {
                    write_standard_format(u, fixed_first<36>(*fixed));
                    fixed->back() = 0;
                } else {
                    throw std::invalid_argument("uuid:write: output span size is small");
                }
            } else {
                if (auto fixed = try_fixed<36>(s); fixed) {
                    write_standard_format(u, *fixed);
                } else {
                    throw std::invalid_argument("uuid:write: output span size is small");
                }
            }
        }
    };

    using uuid_string = uuid_basic_string<char>;
    using uuid_wstring = uuid_basic_string<wchar_t>;
    using uuid_u8string = uuid_basic_string<char8_t>;
    using uuid_u16string = uuid_basic_string<char16_t>;
    using uuid_u32string = uuid_basic_string<char32_t>;
}
