// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <compare>
#include <cstdint>
#include <functional>
#include <ostream>
#include <span>
#include <stdexcept>
#include <string>
#include "fquuid_types.hpp"
#include "fquuid_string.hpp"
#include "fquuid_binary.hpp"

namespace fquuid
{
    class uuid
    {
        detail::uuid_u64 u_;

    public:
        constexpr uuid() noexcept : u_{} {}

        constexpr explicit uuid(const detail::uuid_u64& u) noexcept : u_(u) {}

        constexpr explicit uuid(std::span<const char> s) {
            detail::uuid_string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const wchar_t> s) {
            detail::uuid_wstring::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const char8_t> s) {
            detail::uuid_u8string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const char16_t> s) {
            detail::uuid_u16string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const char32_t> s) {
            detail::uuid_u32string::parse(u_, s);
        }

        constexpr explicit uuid(const char* s) {
            detail::uuid_string::parse(u_, s);
        }

        constexpr explicit uuid(const wchar_t* s) {
            detail::uuid_wstring::parse(u_, s);
        }

        constexpr explicit uuid(const char8_t* s) {
            detail::uuid_u8string::parse(u_, s);
        }

        constexpr explicit uuid(const char16_t* s) {
            detail::uuid_u16string::parse(u_, s);
        }

        constexpr explicit uuid(const char32_t* s) {
            detail::uuid_u32string::parse(u_, s);
        }

        constexpr explicit uuid(std::span<const uint8_t> bytes) {
            detail::uuid_binary_u8::load_from_bytes(u_, bytes);
        }

        constexpr explicit uuid(std::span<const std::byte> bytes) {
            detail::uuid_binary_byte::load_from_bytes(u_, bytes);
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

        constexpr void write_string(std::span<char> s, string_terminator term = string_terminator::null) const {
            detail::uuid_string::write(u_, s, term);
        }

        constexpr void write_string(std::span<wchar_t> s, string_terminator term = string_terminator::null) const {
            detail::uuid_wstring::write(u_, s, term);
        }

        constexpr void write_string(std::span<char8_t> s, string_terminator term = string_terminator::null) const {
            detail::uuid_u8string::write(u_, s, term);
        }

        constexpr void write_string(std::span<char16_t> s, string_terminator term = string_terminator::null) const {
            detail::uuid_u16string::write(u_, s, term);
        }

        constexpr void write_string(std::span<char32_t> s, string_terminator term = string_terminator::null) const {
            detail::uuid_u32string::write(u_, s, term);
        }

        template <class String = std::string>
        String to_string() const {
            using CharT = typename String::value_type;

            String s(36, 0);
            detail::uuid_basic_string<CharT>::write(u_, s, string_terminator::none);
            return s;
        }

        constexpr void write_bytes(std::span<uint8_t> bytes) const {
            detail::uuid_binary_u8::store_to_bytes(u_, bytes);
        }

        constexpr void write_bytes(std::span<std::byte> bytes) const {
            detail::uuid_binary_byte::store_to_bytes(u_, bytes);
        }

        template <ByteLike ByteT = std::byte>
        constexpr std::array<ByteT, 16> to_bytes() const {
            std::array<ByteT, 16> bytes;
            detail::uuid_basic_binary<ByteT>::store_to_bytes(u_, bytes);
            return bytes;
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
