#include "../String.h"
#include "../CommonExceptions.h"

namespace Core
{
    String::String(Allocator allocator) noexcept
        : data(nullptr),
          size(0),
          capacity(0),
          allocator(allocator)
    {}

    String::String(DeviceSize reserve, Allocator allocator) noexcept
        : String(allocator)
    {
        Reserve(reserve);
    }

    String::~String()
    {
        if(this->data)
            this->allocator.Deallocate(this->data);
    }

    String::String(const String& str)
    {
        String tmp_str(str.data, str.size, str.allocator);

        *this = Move(tmp_str);
    }

    String::String(String&& str) noexcept
        : data(Exchange(str.data, nullptr)),
          size(Exchange(str.size, 0)),
          capacity(Exchange(str.capacity, 0)),
          allocator(str.allocator)
    {}

    String& String::operator=(const String& str)
    {
        *this = String(this->allocator);

        String tmp_str(str.data, str.size, str.allocator);

        *this = Move(tmp_str);

        return *this;
    }

    String& String::operator=(String&& str) noexcept
    {
        this->~String();

        this->data = Exchange(str.data, nullptr);
        this->size = Exchange(str.size, 0);
        this->capacity = Exchange(str.capacity, 0);
        this->allocator = str.allocator;

        return *this;
    }

    String::String(ConstIterator begin, ConstIterator end, Allocator allocator)
        : String(begin.GetAddress(), end.GetAddress() - begin.GetAddress(), allocator)
    {}

    String::String(const Char* input, DeviceSize input_size, Allocator allocator)
        : String(allocator)
    {
        this->Append(input, input_size);
    }

    String::String(const WideChar* input, DeviceSize input_size, Allocator allocator)
        : String(allocator)
    {
        this->Append(input, input_size);
    }

    String::String(const UTF8Char* input, DeviceSize input_size, Allocator allocator)
        : String(allocator)
    {
        this->Append(input, input_size);
    }

    String::String(const UTF16Char* input, DeviceSize input_size, Allocator allocator)
        : String(allocator)
    {
        this->Append(input, input_size);
    }

    String::String(const UTF32Char* input, DeviceSize input_size, Allocator allocator)
        : String(allocator)
    {
        this->Append(input, input_size);
    }

    Bool String::IsEmpty() const noexcept
    {
        return this->size == 0;
    }

    DeviceSize String::GetSize() const noexcept
    {
        return this->size;
    }

    DeviceSize String::GetCapacity() const noexcept
    {
        return this->capacity;
    }

    Allocator String::GetAllocator() const noexcept
    {
        return this->allocator;
    }

    UTF8Char* String::GetData() noexcept
    {
        return this->data;
    }

    const UTF8Char* String::GetData() const noexcept
    {
        return this->data;
    }

    Char* String::GetDataAsNativeChar() noexcept
    {
        return reinterpret_cast<Char*>(this->data);
    }

    const Char* String::GetDataAsNativeChar() const noexcept
    {
        return reinterpret_cast<const Char*>(this->data);
    }

    namespace Detail
    {
        Void StringReserveImpl(DeviceSize reserve, UTF8Char*& str_data, DeviceSize str_size, DeviceSize& str_capacity, Allocator& str_allocator)
        {
            if(str_capacity >= reserve)
                return;

            if(str_data != nullptr && str_allocator.Grow(str_data, reserve)) //try grow
            {
                str_capacity = reserve;
            }
            else //allocate new buffer
            {
                UTF8Char* new_memory = reinterpret_cast<UTF8Char*>(str_allocator.Allocate(String::GetMemoryRequirements(reserve)));

                CopyNonOverlappedMemory(str_data, new_memory, str_size);

                if(str_data != nullptr)
                    str_allocator.Deallocate(str_data);

                str_data = new_memory;
                str_capacity = reserve;
            }
        }
    };

    Void String::Reserve(DeviceSize reserve)
    {
        Detail::StringReserveImpl(reserve, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Clear() noexcept
    {
        this->size = 0;
    }

    Bool String::FlushUnusedReserve() noexcept
    {
        Bool res = false;

        if(!this->data)
            res = true;
        else if(this->size == 0 && this->capacity != 0)
        {
            this->allocator.Deallocate(this->data);

            this->data = nullptr;
            this->capacity = 0;

            res = true;
        }
        else if(this->capacity > this->size)
        {
            res = allocator.Trim(this->data, this->size);
            if(res)
                this->capacity = this->size;
        }

        return res;
    }

    namespace Detail
    {
        template<Character C>
        Void StringPrependImpl(const C* input, DeviceSize input_size, UTF8Char*& str_data, DeviceSize& str_size, DeviceSize& str_capacity, Allocator& str_allocator)
        {
            if(input_size == 0)
                return;

            auto length_res = StringEncoder::GetLength(input, input_size);
            if(length_res.input_offset != input_size)
                throw StringEncoderBadCharacterException(input_size, length_res.input_offset);

            DeviceSize new_size = str_size + length_res.output_size;

            if(str_capacity >= new_size)
            {
                CopyOverlappedMemory(str_data, str_data + length_res.output_size, str_size); //move old data
                StringEncoder::Convert(input, input_size, str_data); //copy new data
            }
            else if(str_data != nullptr && str_allocator.Grow(str_data, new_size))
            {
                CopyOverlappedMemory(str_data, str_data + length_res.output_size, str_size); //move old data
                StringEncoder::Convert(input, input_size, str_data); //copy new data

                str_capacity = new_size;
            }
            else
            {
                UTF8Char* new_memory = reinterpret_cast<UTF8Char*>(str_allocator.Allocate(String::GetMemoryRequirements(new_size)));

                StringEncoder::Convert(input, input_size, new_memory); //copy new data
                CopyNonOverlappedMemory(str_data, new_memory + length_res.output_size, str_size);

                str_capacity = new_size;

                if(str_data != nullptr)
                    str_allocator.Deallocate(str_data);

                str_data = new_memory;
            }

            str_size = new_size;
        }

        template<Character C>
        Void StringAppendImpl(const C* input, DeviceSize input_size, UTF8Char*& str_data, DeviceSize& str_size, DeviceSize& str_capacity, Allocator& str_allocator)
        {
            if(input_size == 0)
                return;

            auto res = StringEncoder::GetLength(input, input_size);
            if(res.input_offset != input_size)
                throw StringEncoderBadCharacterException(input_size, res.input_offset);

            Detail::StringReserveImpl(str_size + res.output_size, str_data, str_size, str_capacity, str_allocator);

            StringEncoder::Convert(input, input_size, str_data + str_size);

            str_size += res.output_size;
        }

        template<Character C>
        Void
        StringInBoundsInsertImpl(String::ConstIterator before_it, const C* input, DeviceSize input_size, UTF8Char*& str_data, DeviceSize& str_size, DeviceSize& str_capacity, Allocator& str_allocator)
        {
            if(input_size == 0)
                return;

            auto length_res = StringEncoder::GetLength(input, input_size);
            if(length_res.input_offset != input_size)
                throw StringEncoderBadCharacterException(input_size, length_res.input_offset);

            DeviceSize new_size = str_size + length_res.output_size;

            UTF8Char* second_part_start_ptr = const_cast<UTF8Char*>(before_it.GetAddress());
            UTF8Char* second_part_final_ptr = second_part_start_ptr + length_res.output_size;
            DeviceSize first_part_size = second_part_start_ptr - str_data;
            DeviceSize second_part_size = str_size - first_part_size;

            if(str_capacity >= new_size)
            {
                CopyOverlappedMemory(second_part_start_ptr, second_part_final_ptr, second_part_size); //move second part
                StringEncoder::Convert(input, input_size, second_part_start_ptr); //copy new data
            }
            else if(str_data != nullptr && str_allocator.Grow(str_data, new_size))
            {
                CopyOverlappedMemory(second_part_start_ptr, second_part_final_ptr, second_part_size); //move second part
                StringEncoder::Convert(input, input_size, second_part_start_ptr); //copy new data

                str_capacity = new_size;
            }
            else
            {
                UTF8Char* new_memory = reinterpret_cast<UTF8Char*>(str_allocator.Allocate(MemoryRequirements{.alignment = alignof(UTF8Char), .size = new_size}));

                CopyNonOverlappedMemory(str_data, new_memory, first_part_size);
                StringEncoder::Convert(input, input_size, new_memory + first_part_size);
                CopyNonOverlappedMemory(second_part_start_ptr, new_memory + first_part_size + length_res.output_size, second_part_size);

                str_capacity = new_size;

                if(str_data != nullptr)
                    str_allocator.Deallocate(str_data);

                str_data = new_memory;
            }

            str_size = new_size;
        }
    };

    Void String::Prepend(const String& str)
    {
        Prepend(str.data, str.size);
    }

    Void String::Prepend(ConstIterator begin, ConstIterator end)
    {
        Prepend(begin.GetAddress(), end.GetAddress() - begin.GetAddress());
    }

    Void String::Prepend(const Char* input, DeviceSize input_size)
    {
        Detail::StringPrependImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Prepend(const WideChar* input, DeviceSize input_size)
    {
        Detail::StringPrependImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Prepend(const UTF8Char* input, DeviceSize input_size)
    {
        Detail::StringPrependImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Prepend(const UTF16Char* input, DeviceSize input_size)
    {
        Detail::StringPrependImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Prepend(const UTF32Char* input, DeviceSize input_size)
    {
        Detail::StringPrependImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Append(const String& str)
    {
        Append(str.data, str.size);
    }

    Void String::Append(ConstIterator begin, ConstIterator end)
    {
        Append(begin.GetAddress(), end.GetAddress() - begin.GetAddress());
    }

    Void String::Append(const Char* input, DeviceSize input_size)
    {
        Detail::StringAppendImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Append(const WideChar* input, DeviceSize input_size)
    {
        Detail::StringAppendImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Append(const UTF8Char* input, DeviceSize input_size)
    {
        Detail::StringAppendImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Append(const UTF16Char* input, DeviceSize input_size)
    {
        Detail::StringAppendImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Append(const UTF32Char* input, DeviceSize input_size)
    {
        Detail::StringAppendImpl(input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Insert(ConstIterator before_it, const String& str)
    {
        Insert(before_it, str.data, str.size);
    }

    Void String::Insert(ConstIterator before_it, ConstIterator begin, ConstIterator end)
    {
        Insert(before_it, begin.GetAddress(), end.GetAddress() - begin.GetAddress());
    }

    Void String::Insert(ConstIterator before_it, const Char* input, DeviceSize input_size)
    {
        if(before_it == GetIterator())
            Prepend(input, input_size);
        else if(before_it == GetSentinel())
            Append(input, input_size);
        else
            Detail::StringInBoundsInsertImpl(before_it, input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Insert(ConstIterator before_it, const WideChar* input, DeviceSize input_size)
    {
        if(before_it == GetIterator())
            Prepend(input, input_size);
        else if(before_it == GetSentinel())
            Append(input, input_size);
        else
            Detail::StringInBoundsInsertImpl(before_it, input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Insert(ConstIterator before_it, const UTF8Char* input, DeviceSize input_size)
    {
        if(before_it == GetIterator())
            Prepend(input, input_size);
        else if(before_it == GetSentinel())
            Append(input, input_size);
        else
            Detail::StringInBoundsInsertImpl(before_it, input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Insert(ConstIterator before_it, const UTF16Char* input, DeviceSize input_size)
    {
        if(before_it == GetIterator())
            Prepend(input, input_size);
        else if(before_it == GetSentinel())
            Append(input, input_size);
        else
            Detail::StringInBoundsInsertImpl(before_it, input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::Insert(ConstIterator before_it, const UTF32Char* input, DeviceSize input_size)
    {
        if(before_it == GetIterator())
            Prepend(input, input_size);
        else if(before_it == GetSentinel())
            Append(input, input_size);
        else
            Detail::StringInBoundsInsertImpl(before_it, input, input_size, this->data, this->size, this->capacity, this->allocator);
    }

    Void String::EraseFirst(ConstIterator end_it) noexcept
    {
        auto addr = end_it.GetAddress();
        assert(addr >= this->data && addr <= (this->data + this->size));

        DeviceSize erase_size = addr - this->data;

        this->size -= erase_size;

        CopyOverlappedMemory(addr, this->data, this->size);
    }

    Void String::EraseLast(ConstIterator first_it) noexcept
    {
        auto addr = first_it.GetAddress();
        assert(addr >= this->data && addr <= (this->data + this->size));

        DeviceSize erase_size = (this->data + this->size) - addr;

        this->size -= erase_size;
    }

    Void String::Erase(ConstIterator begin, ConstIterator end) noexcept
    {
        if(end == GetSentinel())
        {
            EraseLast(begin);
        }
        else if(begin == GetIterator())
        {
            EraseFirst(end);
        }
        else
        {
            DeviceSize erase_size = (end.GetAddress() - begin.GetAddress());

            DeviceSize size_to_move = (this->data + this->size) - end.GetAddress();
            CopyOverlappedMemory(end.GetAddress(), const_cast<UTF8Char*>(begin.GetAddress()), size_to_move);

            this->size -= erase_size;
        }
    }

    String::Iterator String::GetIterator() noexcept
    {
        return Iterator(this->data);
    }

    String::ConstIterator String::GetIterator() const noexcept
    {
        return ConstIterator(this->data);
    }

    String::Iterator String::GetSentinel() noexcept
    {
        return Iterator(this->data + this->size);
    }

    String::ConstIterator String::GetSentinel() const noexcept
    {
        return ConstIterator(this->data + this->size);
    }

    String String::operator+(const String& str)
    {
        String out(this->allocator);
        out.Reserve(this->capacity + str.size);

        out.Append(*this);
        out.Append(str);

        return out;
    }

    String& String::operator+=(const String& str)
    {
        this->Append(str);

        return *this;
    }

    String::ConstIterator String::Find(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        auto ptr = ::Core::Detail::FindInString(this->data, this->size, input, input_size);
        if(ptr == nullptr)
            return GetSentinel();

        return ConstIterator(ptr);
    }

    String::ConstIterator String::Find(const String& str) const noexcept
    {
        return Find(str.data, str.size);
    }

    String::ConstIterator String::FindReverse(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        auto ptr = ::Core::Detail::FindInStringReverse(this->data, this->size, input, input_size);
        if(ptr == nullptr)
            return GetSentinel();

        return ConstIterator(ptr);
    }

    String::ConstIterator String::FindReverse(const String& str) const noexcept
    {
        return FindReverse(str.data, str.size);
    }

    Bool String::StartsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        return ::Core::Detail::StringStartsWith(this->data, this->size, input, input_size);
    }

    Bool String::StartsWith(const String& str) const noexcept
    {
        return StartsWith(str.data, str.size);
    }

    Bool String::EndsWith(const UTF8Char* input, DeviceSize input_size) const noexcept
    {
        return ::Core::Detail::StringEndsWith(this->data, this->size, input, input_size);
    }

    Bool String::EndsWith(const String& str) const noexcept
    {
        return EndsWith(str.data, str.size);
    }

    Bool String::operator==(const String& str) const noexcept
    {
        return ::Core::Detail::CompareStringsEquality(this->data, this->size, str.data, str.size);
    }

    Bool String::operator<(const String& str) const noexcept
    {
        return ::Core::Detail::CompareStringsLexicallyLess(this->data, this->size, str.data, str.size);
    }

    MemoryRequirements String::GetMemoryRequirements(DeviceSize reserve) noexcept
    {
        return MemoryRequirements{.alignment = alignof(UTF8Char), .size = reserve};
    }
};