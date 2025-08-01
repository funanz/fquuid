// Copyright 2025 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstddef>
#include <optional>
#include <span>

namespace fquuid::detail
{
    template <size_t Index, class T, size_t Extent>
    constexpr T& fixed_at(std::span<T, Extent> src)
    {
        static_assert(Extent != std::dynamic_extent, "Source Extent must be fixed length");
        static_assert(Extent > Index, "Index out of range");

        return src[Index];
    }

    template <class T, size_t Extent>
    constexpr T& fixed_front(std::span<T, Extent> src)
    {
        static_assert(Extent != std::dynamic_extent, "Source Extent must be fixed length");
        static_assert(Extent > 0, "Source length is zero");

        return src.front();
    }

    template <class T, size_t Extent>
    constexpr T& fixed_back(std::span<T, Extent> src)
    {
        static_assert(Extent != std::dynamic_extent, "Source Extent must be fixed length");
        static_assert(Extent > 0, "Source length is zero");

        return src.back();
    }

    template <size_t Count, class T, size_t Extent>
    constexpr std::span<T, Count> fixed_first(std::span<T, Extent> src) {
        static_assert(Count != std::dynamic_extent, "Count cannot use std::dynamic_extent");
        static_assert(Extent != std::dynamic_extent, "Source Extent must be fixed length");
        static_assert(Extent >= Count, "Source length is insufficient");

        return src.template first<Count>();
    }

    template <size_t Count, class T, size_t Extent>
    constexpr std::span<T, Count> fixed_last(std::span<T, Extent> src) {
        static_assert(Count != std::dynamic_extent, "Count cannot use std::dynamic_extent");
        static_assert(Extent != std::dynamic_extent, "Source Extent must be fixed length");
        static_assert(Extent >= Count, "Source length is insufficient");

        return src.template last<Count>();
    }

    template <size_t Offset, size_t Count = std::dynamic_extent, class T, size_t Extent>
    constexpr auto fixed_subspan(std::span<T, Extent> src) {
        static_assert(Extent != std::dynamic_extent, "Source Extent must be fixed length");
        static_assert(Extent >= Offset, "Offset is out of range");
        constexpr size_t Size = (Count == std::dynamic_extent) ? Extent - Offset : Count;
        static_assert(Extent - Offset >= Size, "Source length is insufficient");

        return src.template subspan<Offset, Size>();
    }

    template <size_t Count, class T, size_t Extent>
    constexpr std::optional<std::span<T, Count>> try_fixed(std::span<T, Extent> src) {
        static_assert(Count != std::dynamic_extent, "Count cannot use std::dynamic_extent");
        static_assert(Extent == std::dynamic_extent, "Source Extent must be dynamic");

        if (src.size() >= Count)
            return src.template first<Count>();
        else
            return std::nullopt;
    }

    template <size_t Count, class T, size_t Extent>
    constexpr std::optional<std::span<T, Count>> try_fixed_equal(std::span<T, Extent> src) {
        static_assert(Count != std::dynamic_extent, "Count cannot use std::dynamic_extent");
        static_assert(Extent == std::dynamic_extent, "Source Extent must be dynamic");

        if (src.size() == Count)
            return src.template first<Count>();
        else
            return std::nullopt;
    }
}
