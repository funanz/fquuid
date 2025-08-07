// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstdint>
#include <type_traits>

namespace fquuid
{
    namespace detail
    {
        struct uuid_u128
        {
            using value_type = uint64_t;

            value_type v_[2];

            constexpr auto operator <=>(const uuid_u128&) const = default;

            constexpr value_type upper() const noexcept { return v_[0]; }
            constexpr value_type lower() const noexcept { return v_[1]; }

            constexpr void upper(value_type n) noexcept { v_[0] = n; }
            constexpr void lower(value_type n) noexcept { v_[1] = n; }

            constexpr bool is_nil() const noexcept {
                return upper() == 0 && lower() == 0;
            }

            constexpr uint8_t version() const noexcept {
                return (upper() >> 12) & 0x0f;
            }

            constexpr uint8_t variant() const noexcept {
                return (lower() >> 62) & 0x03;
            }

            constexpr int64_t unix_ts_ms() const noexcept {
                return (upper() >> 16) & 0x0000'ffff'ffff'ffff;
            }

            constexpr void version(uint8_t ver) noexcept {
                upper((upper() & 0xffff'ffff'ffff'0fff)
                      | (static_cast<value_type>(ver & 0x0f) << 12));
            }

            constexpr void variant(uint8_t var) noexcept {
                lower((lower() & 0x3fff'ffff'ffff'ffff)
                      | (static_cast<value_type>(var & 0x03) << 62));
            }

            constexpr void unix_ts_ms(int64_t ms) noexcept {
                upper((upper() & 0x0000'0000'0000'ffff) | (ms << 16));
            }
        };
    }

    template <class ByteT>
    concept ByteLike = requires {
        requires std::is_trivially_copyable_v<ByteT>;
        requires sizeof(ByteT) == 1;
        requires !std::is_same_v<ByteT, bool>;
    };

    enum class string_terminator { none, null };
}
