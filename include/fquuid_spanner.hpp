// Copyright 2025 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstddef>
#include <optional>
#include <span>

namespace fquuid::detail
{
    template <size_t Size, class T, size_t Extent>
    constexpr std::span<T, Size> fixed_first(std::span<T, Extent> src) {
        static_assert(Size != std::dynamic_extent, "Size cannot use dynamic_extent");
        static_assert(Extent != std::dynamic_extent, "Extent must be fixed length");
        static_assert(Extent >= Size, "Source length is insufficient");

        return src.template first<Size>();
    }

    template <size_t Size, class T, size_t Extent>
    constexpr std::span<T, Size> fixed_last(std::span<T, Extent> src) {
        static_assert(Size != std::dynamic_extent, "Size cannot use dynamic_extent");
        static_assert(Extent != std::dynamic_extent, "Extent must be fixed length");
        static_assert(Extent >= Size, "Source length is insufficient");

        return src.template last<Size>();
    }

    template <size_t Offset, size_t Size, class T, size_t Extent>
    constexpr std::span<T, Size> fixed_subspan(std::span<T, Extent> src) {
        static_assert(Size != std::dynamic_extent, "Size cannot use dynamic_extent");
        static_assert(Extent != std::dynamic_extent, "Extent must be fixed length");
        static_assert(Extent >= (Offset + Size), "Source length is insufficient");

        return src.template subspan<Offset, Size>();
    }

    template <size_t Size, class T, size_t Extent>
    constexpr std::optional<std::span<T, Size>> try_fixed(std::span<T, Extent> src) {
        static_assert(Size != std::dynamic_extent, "Size cannot use dynamic_extent");
        static_assert(Extent == std::dynamic_extent, "Extent must be dynamic");

        if (src.size() >= Size)
            return src.template first<Size>();
        else
            return std::nullopt;
    }

    template <size_t Size, class T, size_t Extent>
    constexpr std::optional<std::span<T, Size>> try_fixed_equal(std::span<T, Extent> src) {
        static_assert(Size != std::dynamic_extent, "Size cannot use dynamic_extent");
        static_assert(Extent == std::dynamic_extent, "Extent must be dynamic");

        if (src.size() == Size)
            return src.template first<Size>();
        else
            return std::nullopt;
    }
}
