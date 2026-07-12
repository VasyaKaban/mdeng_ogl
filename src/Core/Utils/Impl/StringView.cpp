#include "../StringView.h"
#include "../String.h"
#include "../CommonExceptions.h"

namespace Core
{

    StringView::StringView() noexcept
        : data(nullptr),
          size(0)
    {}

    StringView::StringView(const UTF8Char* input, DeviceSize input_size)
        : data(input),
          size(input_size)
    {
        auto res = StringEncoder::GetLength(input, input_size);
        if(res.input_offset != input_size)
            throw StringEncoderBadCharacterException(input_size, res.input_offset);
    }

    StringView::StringView(const String& str) noexcept
        : StringView(str.GetData(), str.GetSize())
    {}

    StringView::StringView(Iterator begin, Iterator end) noexcept
        : data(begin.GetAddress()),
          size(end.GetAddress() - begin.GetAddress())
    {}

    Bool StringView::IsEmpty() const noexcept
    {
        return this->size == 0;
    }

    DeviceSize StringView::GetSize() const noexcept
    {
        return this->size;
    }

    const UTF8Char* StringView::GetData() const noexcept
    {
        return this->data;
    }

    const Char* StringView::GetDataAsNativeChar() const noexcept
    {
        return reinterpret_cast<const Char*>(this->data);
    }

    StringView::Iterator StringView::GetIterator() const noexcept
    {
        return Iterator(this->data);
    }

    StringView::Iterator StringView::GetSentinel() const noexcept
    {
        return Iterator(this->data + this->size);
    }

    StringView::Iterator StringView::Find(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        auto ptr = ::Core::Detail::FindInString(this->data, this->size, input, input_size);
        if(ptr == nullptr)
            return GetSentinel();

        return Iterator(ptr);
    }

    StringView::Iterator StringView::Find(const StringView& str) const noexcept
    {
        return Find(str.data, str.size);
    }

    StringView::Iterator StringView::FindReverse(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        auto ptr = ::Core::Detail::FindInStringReverse(this->data, this->size, input, input_size);
        if(ptr == nullptr)
            return GetSentinel();

        return Iterator(ptr);
    }

    StringView::Iterator StringView::FindReverse(const StringView& str) const noexcept
    {
        return FindReverse(str.data, str.size);
    }

    Bool StringView::StartsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        return ::Core::Detail::StringStartsWith(this->data, this->size, input, input_size);
    }

    Bool StringView::StartsWith(const StringView& str) const noexcept
    {
        return StartsWith(str.data, str.size);
    }

    Bool StringView::EndsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        return ::Core::Detail::StringEndsWith(this->data, this->size, input, input_size);
    }

    Bool StringView::EndsWith(const StringView& str) const noexcept
    {
        return EndsWith(str.data, str.size);
    }

    Bool StringView::operator==(const StringView& str) const noexcept
    {
        return ::Core::Detail::CompareStringsEquality(this->data, this->size, str.data, str.size);
    }

    Bool StringView::operator<(const StringView& str) const noexcept
    {
        return ::Core::Detail::CompareStringsLexicallyLess(this->data, this->size, str.data, str.size);
    }

};