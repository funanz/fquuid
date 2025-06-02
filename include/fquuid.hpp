// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <cstdint>
#include <type_traits>

namespace fquuid
{
    template <class ByteT>
    concept ByteLike = requires {
        requires std::is_trivially_copyable_v<ByteT>;
        requires sizeof(ByteT) == 1;
        requires !std::is_same_v<ByteT, bool>;
    };

    using uuid_u64 = std::array<uint64_t, 2>;

    enum class string_terminator { none, null };
}

#include "fquuid_random.hpp"
#include "fquuid_generator.hpp"
#include "fquuid_string.hpp"
#include "fquuid_binary.hpp"
#include "fquuid_uuid.hpp"
