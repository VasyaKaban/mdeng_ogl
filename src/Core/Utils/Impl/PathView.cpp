#include "../PathView.h"
#include "../Ranges.hpp"

namespace Core
{
    PathView::PathView() noexcept
        : data()
    {}

    PathView::PathView(const Path& path) noexcept
        : data(path.GetData())
    {}

    PathView::PathView(const Iterator& begin, const Iterator& end) noexcept
        : data(begin.GetDataIterator(), end.GetDataIterator())
    {}

    PathView::~PathView()
    {}

    PathView::PathView(const PathView& path) noexcept
        : data(path.data)
    {}

    PathView::PathView(PathView&& path) noexcept
        : data(path.data)
    {}

    PathView& PathView::operator=(const PathView& path) noexcept
    {
        this->data = path.data;

        return *this;
    }

    PathView& PathView::operator=(PathView&& path) noexcept
    {
        this->data = path.data;

        return *this;
    }

    StringView PathView::GetExtension() const noexcept
    {
        if(IsEmpty() || IsAbsolute())
            return StringView();

        auto it = this->data.GetSentinel();
        it--;
        while((*it).utf32 != U'.')
        {
            it--;
        }

        return StringView(it, this->data.GetSentinel());
    }

    StringView PathView::GetFileName() const noexcept
    {
        if(IsEmpty() || IsAbsolute())
            return StringView();

        auto it = this->data.GetSentinel();
        it--;
        while((*it).utf32 != U'/')
        {
            it--;
        }

        return StringView(it, this->data.GetSentinel());
    }

    Bool PathView::IsAbsolute() const noexcept
    {
        return this->data.StartsWith(u8"/");
    }

    Bool PathView::IsEmpty() const noexcept
    {
        return this->data.IsEmpty();
    }

    DeviceSize PathView::GetSize() const noexcept
    {
        return this->data.GetSize();
    }

    StringView PathView::GetData() const noexcept
    {
        return this->data;
    }

    PathView::Iterator PathView::GetIterator() const noexcept
    {
        return Iterator(this->data, this->data.GetIterator());
    }

    PathView::Iterator PathView::GetSentinel() const noexcept
    {
        return Iterator(this->data, this->data.GetSentinel());
    }
};