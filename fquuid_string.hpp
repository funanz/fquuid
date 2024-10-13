#pragma once
#include "fquuid.hpp"

namespace fquuid
{
    class uuid_string
    {
        using uuid_array = std::array<uint64_t, 2>;

        static constexpr auto hex_to_u4_table = [] {
            std::array<uint8_t, 256> a;
            for (size_t i = 0; i < a.size(); i++) {
                if ('0' <= i && i <= '9')
                    a[i] =  i - '0';
                else if ('A' <= i && i <= 'F')
                    a[i] =  i - 'A' + 10;
                else if ('a' <= i && i <= 'f')
                    a[i] =  i - 'a' + 10;
                else
                    a[i] = 0xff;
            }
            return a;
        }();

        static constexpr uint64_t hex_to_u4(char c) {
            static_assert(hex_to_u4_table.size() == 256);

            auto u4 = hex_to_u4_table[static_cast<size_t>(c) & 0xff];
            if (u4 == 0xff)
                throw std::invalid_argument("invalid hex character [0-9a-fA-F]");

            return u4;
        }

        template <int Bit>
        requires (Bit == 64 || Bit == 48 || Bit == 32 ||Bit == 16)
        static constexpr uint64_t load_hex(std::span<const char> s) {
            if (s.size() < Bit / 4)
                throw std::invalid_argument("uuid::load_hex() span size is small");

            constexpr size_t Adj = 16 - Bit / 4;
            uint64_t x = 0;
            if constexpr (Bit == 64) {
                x |= (hex_to_u4(s[0 - Adj]) << 60 |
                      hex_to_u4(s[1 - Adj]) << 56 |
                      hex_to_u4(s[2 - Adj]) << 52 |
                      hex_to_u4(s[3 - Adj]) << 48);
            }
            if constexpr (Bit >= 48) {
                x |= (hex_to_u4(s[4 - Adj]) << 44 |
                      hex_to_u4(s[5 - Adj]) << 40 |
                      hex_to_u4(s[6 - Adj]) << 36 |
                      hex_to_u4(s[7 - Adj]) << 32);
            }
            if constexpr (Bit >= 32) {
                x |= (hex_to_u4(s[8 - Adj]) << 28 |
                      hex_to_u4(s[9 - Adj]) << 24 |
                      hex_to_u4(s[10 - Adj]) << 20 |
                      hex_to_u4(s[11 - Adj]) << 16);
            }
            if constexpr (Bit >= 16) {
                x |= (hex_to_u4(s[12 - Adj]) << 12 |
                      hex_to_u4(s[13 - Adj]) << 8 |
                      hex_to_u4(s[14 - Adj]) << 4 |
                      hex_to_u4(s[15 - Adj]));
            }
            return x;
        }

        static constexpr char u4_to_hex(uint64_t u4) {
            constexpr auto u4_to_hex_table = "0123456789abcdef";

            return u4_to_hex_table[u4 & 0xf];
        }

        template <int Bit>
        requires (Bit == 64 || Bit == 48 || Bit == 32 ||Bit == 16)
        static constexpr void store_hex(uint64_t x, std::span<char> s) {
            if (s.size() < Bit / 4)
                throw std::invalid_argument("uuid::store_hex() span size is small");

            constexpr size_t Adj = 16 - Bit / 4;
            if constexpr (Bit == 64) {
                s[0 - Adj] = u4_to_hex(x >> 60);
                s[1 - Adj] = u4_to_hex(x >> 56);
                s[2 - Adj] = u4_to_hex(x >> 52);
                s[3 - Adj] = u4_to_hex(x >> 48);
            }
            if constexpr (Bit >= 48) {
                s[4 - Adj] = u4_to_hex(x >> 44);
                s[5 - Adj] = u4_to_hex(x >> 40);
                s[6 - Adj] = u4_to_hex(x >> 36);
                s[7 - Adj] = u4_to_hex(x >> 32);
            }
            if constexpr (Bit >= 32) {
                s[8 - Adj] = u4_to_hex(x >> 28);
                s[9 - Adj] = u4_to_hex(x >> 24);
                s[10 - Adj] = u4_to_hex(x >> 20);
                s[11 - Adj] = u4_to_hex(x >> 16);
            }
            if constexpr (Bit >= 16) {
                s[12 - Adj] = u4_to_hex(x >> 12);
                s[13 - Adj] = u4_to_hex(x >> 8);
                s[14 - Adj] = u4_to_hex(x >> 4);
                s[15 - Adj] = u4_to_hex(x);
            }
        }

        static constexpr bool is_standerd_format(std::span<const char> s) {
            if (s.size() < 36) return false;

            return (s[ 8] == '-' && s[13] == '-' &&
                    s[18] == '-' && s[23] == '-');
        }

    public:
        static constexpr void parse(uuid_array& u, std::span<const char> s) {
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

        static constexpr void to_string(const uuid_array& u, std::span<char> s, bool null_terminate) {
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
