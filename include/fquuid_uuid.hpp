// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <compare>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <ostream>
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

        constexpr explicit uuid(std::span<const wchar_t> s) {
            uuid_wstring::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const char8_t> s) {
            uuid_u8string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const char16_t> s) {
            uuid_u16string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const char32_t> s) {
            uuid_u32string::parse(u_, s);
        }

        constexpr explicit uuid(const char* s) {
            uuid_string::parse(u_, s);
        }

        constexpr explicit uuid(const wchar_t* s) {
            uuid_wstring::parse(u_, s);
        }

        constexpr explicit uuid(const char8_t* s) {
            uuid_u8string::parse(u_, s);
        }

        constexpr explicit uuid(const char16_t* s) {
            uuid_u16string::parse(u_, s);
        }

        constexpr explicit uuid(const char32_t* s) {
            uuid_u32string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const uint8_t> bytes) {
            uuid_binary_u8::load_from_bytes(u_, bytes);
        }

        constexpr explicit uuid(std::span<const std::byte> bytes) {
            uuid_binary_byte::load_from_bytes(u_, bytes);
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

        constexpr bool is_nil() const noexcept {
            return u_[0] == 0 && u_[1] == 0;
        }

        constexpr uint8_t get_version() const noexcept {
            return (u_[0] >> 12) & 0xf;
        }

        constexpr void write_string_without_null(std::span<char> s) const {
            uuid_string::write(u_, s, false);
        }

        constexpr void write_string_without_null(std::span<wchar_t> s) const {
            uuid_wstring::write(u_, s, false);
        }

        constexpr void write_string_without_null(std::span<char8_t> s) const {
            uuid_u8string::write(u_, s, false);
        }

        constexpr void write_string_without_null(std::span<char16_t> s) const {
            uuid_u16string::write(u_, s, false);
        }

        constexpr void write_string_without_null(std::span<char32_t> s) const {
            uuid_u32string::write(u_, s, false);
        }

        constexpr void write_string(std::span<char> s) const {
            uuid_string::write(u_, s, true);
        }

        constexpr void write_string(std::span<wchar_t> s) const {
            uuid_wstring::write(u_, s, true);
        }

        constexpr void write_string(std::span<char8_t> s) const {
            uuid_u8string::write(u_, s, true);
        }

        constexpr void write_string(std::span<char16_t> s) const {
            uuid_u16string::write(u_, s, true);
        }

        constexpr void write_string(std::span<char32_t> s) const {
            uuid_u32string::write(u_, s, true);
        }

        template <class String = std::string>
        String to_string() const {
            using CharT = typename String::value_type;

            String s(36, 0);
            uuid_basic_string<CharT>::write(u_, s, false);
            return s;
        }

        void println() const {
            std::array<char, 37> a;
            uuid_string::write(u_, a, true);
            puts(a.data());
        }

        constexpr void write_bytes(std::span<uint8_t> bytes) const {
            uuid_binary_u8::store_to_bytes(u_, bytes);
        }

        constexpr void write_bytes(std::span<std::byte> bytes) const {
            uuid_binary_byte::store_to_bytes(u_, bytes);
        }

        size_t hash() const noexcept {
            std::hash<uint64_t> h;

            return h(u_[0]) ^ h(u_[1]);
        }
    };

    template <class CharT, class Traits>
    auto& operator <<(std::basic_ostream<CharT, Traits>& os, const uuid& u) {
        std::array<CharT, 37> buf;
        u.write_string(buf);
        return os << buf.data();
    }
}

namespace std
{
    template<>
    struct hash<fquuid::uuid>
    {
        size_t operator()(const fquuid::uuid& u) const noexcept {
            return u.hash();
        }
    };
}
