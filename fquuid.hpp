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
#include "fquuid_string.hpp"

namespace fquuid
{
    class uuid
    {
        using uuid_array = std::array<uint64_t, 2>;

        uuid_array u_;

    public:
        constexpr uuid() noexcept : u_{} {}

        constexpr explicit uuid(std::span<const char> s) {
            uuid_string::parse(u_, s);
        }

        constexpr explicit uuid(const std::string& s) {
            uuid_string::parse(u_, std::span(s.c_str(), s.length()));
        }

        constexpr explicit uuid(const char* s) {
            if (s == nullptr)
                throw std::invalid_argument("uuid::uuid() argument is nullptr");

            auto len = uuid_string::strlen(s);
            uuid_string::parse(u_, std::span(s, len));
        }

        constexpr explicit uuid(std::span<const uint8_t> bytes) {
            uuid_bytes::load_from_bytes(u_, bytes);
        }

        explicit uuid(const uuid_array& u) noexcept : u_(u) {}

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
            uuid_string::to_string(u_, s, false);
        }

        constexpr void to_string_z(std::span<char> s) const {
            uuid_string::to_string(u_, s, true);
        }

        std::string to_string() const {
            std::string s(36, 0);
            uuid_string::to_string(u_, s, false);
            return s;
        }

        void println() const {
            std::array<char, 37> a;
            uuid_string::to_string(u_, a, true);
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
    struct hash<fquuid::uuid>
    {
    public:
        size_t operator()(const fquuid::uuid& u) const noexcept {
            return u.hash();
        }
    };
}
