// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <cstdint>
#include <type_traits>

namespace fquuid
{
    namespace detail
    {
        using uuid_u64 = std::array<uint64_t, 2>;
    }

    template <class ByteT>
    concept ByteLike = requires {
        requires std::is_trivially_copyable_v<ByteT>;
        requires sizeof(ByteT) == 1;
        requires !std::is_same_v<ByteT, bool>;
    };

    enum class string_terminator { none, null };
}
