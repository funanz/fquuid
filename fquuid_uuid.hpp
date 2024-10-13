#pragma once
#include <array>
#include <compare>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <span>
#include <stdexcept>
#include <string>
#include "fquuid_string.hpp"
#include "fquuid_binary.hpp"

namespace fquuid
{
    class uuid
    {
        uuid_u64 u_;

    public:
        constexpr uuid() noexcept : u_{} {}

        constexpr explicit uuid(const uuid_u64& u) noexcept : u_(u) {}

        constexpr explicit uuid(std::span<const char> s) {
            uuid_string::parse(u_, s);
        }

        constexpr explicit uuid(const std::string& s) {
            uuid_string::parse(u_, s);
        }

        constexpr explicit uuid(const char* s) {
            if (s == nullptr)
                throw std::invalid_argument("uuid::uuid() argument is nullptr");

            auto len = uuid_string::strlen(s);
            uuid_string::parse(u_, std::span(s, len));
        }

        constexpr explicit uuid(std::span<const uint8_t> bytes) {
            uuid_binary::load_from_bytes(u_, bytes);
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
            uuid_binary::store_to_bytes(u_, bytes);
        }

        size_t hash() const noexcept {
            std::hash<uint64_t> h;

            return h(u_[0]) ^ h(u_[1]);
        }
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
