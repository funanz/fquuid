#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include "fquuid_random.hpp"

namespace fquuid
{
    class uuid
    {
        using uuid_array = std::array<uint64_t, 2>;

        uuid_array u_;

    public:
        static uuid v4() {
            uuid_random rng;
            return uuid_generator::v4(rng);
        }

        template <class RNG>
        static uuid v4(RNG& rng) {
            return uuid_generator::v4(rng);
        }

        static uuid v7() {
            uuid_random rng;
            return uuid_generator::v7(rng);
        }

        template <class RNG>
        static uuid v7(RNG& rng) {
            return uuid_generator::v7(rng);
        }

        template <class RNG>
        static uuid v7(RNG& rng, int64_t ms) {
            return uuid_generator::v7(rng, ms);
        }

        constexpr uuid() noexcept : u_{} {}

        constexpr explicit uuid(std::span<const char> s) {
            uuid_parser::parse(u_, s);
        }

        constexpr explicit uuid(const std::string& s) {
            uuid_parser::parse(u_, std::span(s.c_str(), s.length()));
        }

        constexpr explicit uuid(const char* s) {
            if (s == nullptr)
                throw std::invalid_argument("uuid::uuid() argument is nullptr");

            auto len = uuid_parser::strlen(s);
            uuid_parser::parse(u_, std::span(s, len));
        }

        constexpr explicit uuid(std::span<const uint8_t> bytes) {
            uuid_bytes::load_from_bytes(u_, bytes);
        }

        constexpr bool operator ==(const uuid& r) const noexcept {
            return u_ == r.u_;
        }

        constexpr bool operator !=(const uuid& r) const noexcept {
            return u_ != r.u_;
        }

        constexpr auto operator <=>(const uuid& r) const noexcept {
            return u_ <=> r.u_;
        }

        constexpr bool is_zero() const noexcept {
            return u_[0] == 0 && u_[1] == 0;
        }

        constexpr uint8_t get_version() const noexcept {
            return (u_[0] >> 12) & 0xf;
        }

        constexpr void to_string(std::span<char> s) const {
            uuid_parser::to_string(u_, s, false);
        }

        constexpr void to_string_z(std::span<char> s) const {
            uuid_parser::to_string(u_, s, true);
        }

        std::string to_string() const {
            std::string s(36, 0);
            uuid_parser::to_string(u_, s, false);
            return s;
        }

        void println() const {
            std::array<char, 37> a;
            uuid_parser::to_string(u_, a, true);
            puts(a.data());
        }

        constexpr void to_bytes(std::span<uint8_t> bytes) const {
            uuid_bytes::store_to_bytes(u_, bytes);
        }

        size_t hash() const noexcept {
            std::hash<uint64_t> h;

            return h(u_[0]) ^ h(u_[1]);
        }

    private:
        explicit uuid(const uuid_array& u) noexcept : u_(u) {}

        class uuid_generator
        {
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
            template <class RNG>
            static uuid v4(RNG& rng) {
                std::uniform_int_distribution<uint64_t> dist64;
                uuid_array u;
                u[0] = dist64(rng);
                u[1] = dist64(rng);

                set_version(u, 4);
                set_variant(u, 0b10);

                return uuid{u};
            }

            template <class RNG>
            static uuid v7(RNG& rng) {
                return v7(rng, get_utc_now());
            }

            template <class RNG>
            static uuid v7(RNG& rng, int64_t ms) {
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

        class uuid_parser
        {
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

        class uuid_bytes
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
            static constexpr void load_from_bytes(uuid_array& u, std::span<const uint8_t> bytes) {
                if (bytes.size() < 16)
                    throw std::invalid_argument("uuid::load_from_bytes() input span size is small");

                u[0] = load_u64(bytes.subspan(0, 8));
                u[1] = load_u64(bytes.subspan(8, 8));
            }

            static constexpr void store_to_bytes(const uuid_array& u, std::span<uint8_t> bytes) {
                if (bytes.size() < 16)
                    throw std::invalid_argument("uuid::store_to_bytes() output span size is small");

                store_u64(u[0], bytes.subspan(0, 8));
                store_u64(u[1], bytes.subspan(8, 8));
            }
        };
    };
}

namespace std
{
    template<>
    class hash<fquuid::uuid>
    {
    public:
        size_t operator()(const fquuid::uuid& u) const noexcept {
            return u.hash();
        }
    };
}
