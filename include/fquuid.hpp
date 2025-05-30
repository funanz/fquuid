// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <array>
#include <cstdint>

namespace fquuid
{
    using uuid_u64 = std::array<uint64_t, 2>;
}

#include "fquuid_random.hpp"
#include "fquuid_generator.hpp"
#include "fquuid_string.hpp"
#include "fquuid_binary.hpp"
#include "fquuid_uuid.hpp"
