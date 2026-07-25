#pragma once

#include "Impl/PathCommon.h"
#include "Path.h"

namespace Core
{
    //Path rules for PathView do not work
    //It can have trailing / and .
    class CORE_API PathView
    {
    public:
        using Iterator = Detail::PathPartIterator;

        PathView() noexcept;
        PathView(const Path& path) noexcept;
        PathView(const Iterator& begin, const Iterator& end) noexcept;
        ~PathView();
        PathView(const PathView& path) noexcept;
        PathView(PathView&& path) noexcept;
        PathView& operator=(const PathView& path) noexcept;
        PathView& operator=(PathView&& path) noexcept;

        StringView GetExtension() const noexcept;
        StringView GetFileName() const noexcept;
        Bool IsAbsolute() const noexcept;

        Bool IsEmpty() const noexcept;
        DeviceSize GetSize() const noexcept;
        StringView GetData() const noexcept;

        Iterator GetIterator() const noexcept;
        Iterator GetSentinel() const noexcept;
    private:
        StringView data;
    };

    //std compat
    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, PathView>
    auto begin(T&& path) noexcept
    {
        return Forward(path).GetIterator();
    }

    template<typename T>
    requires SameAs<DropConstVolatileReference<T>, PathView>
    auto end(T&& path) noexcept
    {
        return Forward(path).GetSentinel();
    }
};