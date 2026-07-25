#include "../Path.h"
#include "../Types.hpp"
#include "../StringView.h"
#include "../Ranges.hpp"

namespace Core
{
    namespace Detail
    {
        enum PathMatrixIndex : UInt32
        {
            Part = 0,
            Back = 1,
            Current = 2,
            Root = 3,
            Empty = 4,
            PathMatrixIndexSize
        };

        using ConcatFunction = Void (*)(String& current, const StringView& input);

        Void PathMatrixAppendToPath(String& current, const StringView& input)
        {
            current.Append(u8"/");
            current.Append(input.GetIterator(), input.GetSentinel());
        };

        Void PathMatrixNoOp(String& current, const StringView& input)
        {
            //noop
            return;
        };

        Void PathMatrixCopyContent(String& current, const StringView& input)
        {
            current.Append(input.GetIterator(), input.GetSentinel());
        };

        Void PathMatrixAssert(String& current, const StringView& input)
        {
            assert(false);
        };

        Void PathMatrixBacktrace(String& current, const StringView& input)
        {
            auto it = current.FindReverse(u8"/");
            if(it == current.GetSentinel()) //no slash -> empty
                current.Clear();
            else
                current.EraseLast(++it);
        };

        constexpr static ConcatFunction PathConcatMatrix[PathMatrixIndex::PathMatrixIndexSize][PathMatrixIndex::PathMatrixIndexSize] = {
            //Part
            {
                /*[PathMatrixIndex::Part][PathMatrixIndex::Part]*/ PathMatrixAppendToPath,
                /*PathConcatMatrix[PathMatrixIndex::Part][PathMatrixIndex::Back]*/ PathMatrixBacktrace,
                /*PathConcatMatrix[PathMatrixIndex::Part][PathMatrixIndex::Current]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Part][PathMatrixIndex::Root]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Part][PathMatrixIndex::Empty]*/ PathMatrixNoOp,
            },
            //Back
            {
                /*PathConcatMatrix[PathMatrixIndex::Back][PathMatrixIndex::Part]*/ PathMatrixAppendToPath,
                /*PathConcatMatrix[PathMatrixIndex::Back][PathMatrixIndex::Back]*/ PathMatrixAppendToPath,
                /*PathConcatMatrix[PathMatrixIndex::Back][PathMatrixIndex::Current]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Back][PathMatrixIndex::Root]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Back][PathMatrixIndex::Empty]*/ PathMatrixNoOp,
            },
            //Current
            {
                /*PathConcatMatrix[PathMatrixIndex::Current][PathMatrixIndex::Part]*/ PathMatrixAssert,
                /*PathConcatMatrix[PathMatrixIndex::Current][PathMatrixIndex::Back]*/ PathMatrixAssert,
                /*PathConcatMatrix[PathMatrixIndex::Current][PathMatrixIndex::Current]*/ PathMatrixAssert,
                /*PathConcatMatrix[PathMatrixIndex::Current][PathMatrixIndex::Root]*/ PathMatrixAssert,
                /*PathConcatMatrix[PathMatrixIndex::Current][PathMatrixIndex::Empty]*/ PathMatrixAssert,
            },
            //Root
            {
                /*PathConcatMatrix[PathMatrixIndex::Root][PathMatrixIndex::Part]*/ PathMatrixCopyContent,
                /*PathConcatMatrix[PathMatrixIndex::Root][PathMatrixIndex::Back]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Root][PathMatrixIndex::Current]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Root][PathMatrixIndex::Root]*/ PathMatrixNoOp,
                /*PathConcatMatrix[PathMatrixIndex::Root][PathMatrixIndex::Empty]*/ PathMatrixNoOp,
            },
            //Empty
            {/*PathConcatMatrix[PathMatrixIndex::Empty][PathMatrixIndex::Part]*/ PathMatrixCopyContent,
             /*PathConcatMatrix[PathMatrixIndex::Empty][PathMatrixIndex::Back]*/ PathMatrixCopyContent,
             /*PathConcatMatrix[PathMatrixIndex::Empty][PathMatrixIndex::Current]*/ PathMatrixNoOp,
             /*PathConcatMatrix[PathMatrixIndex::Empty][PathMatrixIndex::Root]*/ PathMatrixCopyContent,
             /*PathConcatMatrix[PathMatrixIndex::Empty][PathMatrixIndex::Empty]*/ PathMatrixNoOp},
        };

        static void SkipPathMultipleSlashes(StringView& str) noexcept
        {
            for(auto it = str.GetIterator(); it != str.GetSentinel(); it++)
            {
                if((*it).utf32 != U'/')
                {
                    str = StringView(it, str.GetSentinel());
                    return;
                }
            }

            str = StringView();
        }

        //result restrictions:
        //no tail /(exception -> root)
        //no .
        static void ConcatPaths(String& current, StringView input)
        {
            current.Reserve(current.GetSize() + input.GetSize());

            while(!input.IsEmpty())
            {
                StringView token;
                PathMatrixIndex token_index = PathMatrixIndex::Empty;
                if(input.StartsWith(u8"/"))
                {
                    token_index = PathMatrixIndex::Root;
                    token = u8"/";
                    SkipPathMultipleSlashes(input);
                }
                else if(input.StartsWith(u8"."))
                {
                    if(input.StartsWith(u8"./"))
                    {
                        token_index = PathMatrixIndex::Current;
                        token = u8".";
                        input = StringView(AdvanceForward(input.GetIterator(), 2), input.GetSentinel());
                        SkipPathMultipleSlashes(input);
                    }
                    else if(input == u8".")
                    {
                        token_index = PathMatrixIndex::Current;
                        token = u8".";
                        input = StringView();
                    }
                    else if(input.StartsWith(u8"../"))
                    {
                        token_index = PathMatrixIndex::Back;
                        token = u8"..";
                        input = StringView(AdvanceForward(input.GetIterator(), 3), input.GetSentinel());
                        SkipPathMultipleSlashes(input);
                    }
                    else if(input == u8"..")
                    {
                        token_index = PathMatrixIndex::Back;
                        token = u8"..";
                        input = StringView();
                    }
                    else
                    {
                        token_index = PathMatrixIndex::Part;

                        auto it = input.Find(u8"/");
                        if(it == input.GetSentinel())
                        {
                            token = input;
                            input = StringView();
                        }
                        else
                        {
                            token = StringView(input.GetIterator(), it);
                            input = StringView(it, input.GetSentinel());
                            SkipPathMultipleSlashes(input);
                        }
                    }
                }
                else
                {
                    token_index = PathMatrixIndex::Part;

                    auto it = input.Find(u8"/");
                    if(it == input.GetSentinel())
                    {
                        token = input;
                        input = StringView();
                    }
                    else
                    {
                        token = StringView(input.GetIterator(), it);
                        input = StringView(it, input.GetSentinel());
                        SkipPathMultipleSlashes(input);
                    }
                }

                PathMatrixIndex current_token_index = PathMatrixIndex::Empty;
                if(!current.IsEmpty())
                {
                    if(current.EndsWith(u8"/"))
                        current_token_index = PathMatrixIndex::Root;
                    else if(current.EndsWith(u8"/..") || current == u8"..")
                        current_token_index = PathMatrixIndex::Back;
                    else if(current.EndsWith(u8"/.") || current == u8".")
                        current_token_index = PathMatrixIndex::Current;
                    else
                        current_token_index = PathMatrixIndex::Part;
                }

                PathConcatMatrix[current_token_index][token_index](current, token);
            }
        }
    };

    Path::Path(Allocator allocator)
        : data(allocator)
    {}

    Path::Path(DeviceSize reserve, Allocator allocator)
        : data(reserve, allocator)
    {}

    Path::~Path()
    {}

    Path::Path(const Path& path)
        : data(path.data)
    {}

    Path::Path(Path&& path) noexcept
        : data(Move(path.data))
    {}

    Path& Path::operator=(const Path& path)
    {
        this->data = path.data;

        return *this;
    }

    Path& Path::operator=(Path&& path) noexcept
    {
        this->data = Move(path.data);

        return *this;
    }

    Path::Path(const StringView& str, Allocator allocator)
        : data(str.GetSize(), allocator)
    {
        Detail::ConcatPaths(this->data, str);
    }

    Path& Path::operator=(StringView str)
    {
        this->data.Clear();

        Detail::ConcatPaths(this->data, str);

        return *this;
    }

    Path::Path(StringView::Iterator begin, StringView::Iterator end, Allocator allocator)
        : Path(StringView(begin, end), allocator)
    {}

    Path::Path(Iterator begin, Iterator end, Allocator allocator)
        : Path(StringView(begin.GetDataIterator(), end.GetDataIterator()))
    {}

    Path::Path(const Char* input, DeviceSize input_size, Allocator allocator)
        : Path(reinterpret_cast<const UTF8Char*>(input), input_size, allocator)
    {}

    Path::Path(const WideChar* input, DeviceSize input_size, Allocator allocator)
        : data(allocator)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));
    }

    Path::Path(const UTF8Char* input, DeviceSize input_size, Allocator allocator)
        : data(input_size, allocator)
    {
        Detail::ConcatPaths(this->data, StringView(input, input_size));
    }

    Path::Path(const UTF16Char* input, DeviceSize input_size, Allocator allocator)
        : data(allocator)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));
    }

    Path::Path(const UTF32Char* input, DeviceSize input_size, Allocator allocator)
        : data(allocator)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));
    }

    Path& Path::Append(const Path& path)
    {
        Detail::ConcatPaths(this->data, path.GetData());

        return *this;
    }

    Path& Path::Append(Iterator begin, Iterator end)
    {
        Detail::ConcatPaths(this->data, StringView(begin.GetDataIterator(), end.GetDataIterator()));

        return *this;
    }

    Path& Path::Append(const Char* input, DeviceSize input_size)
    {
        return Append(reinterpret_cast<const UTF8Char*>(input), input_size);
    }

    Path& Path::Append(const WideChar* input, DeviceSize input_size)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));

        return *this;
    }

    Path& Path::Append(const UTF8Char* input, DeviceSize input_size)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));

        return *this;
    }

    Path& Path::Append(const UTF16Char* input, DeviceSize input_size)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));

        return *this;
    }

    Path& Path::Append(const UTF32Char* input, DeviceSize input_size)
    {
        String tmp(input, input_size);
        Detail::ConcatPaths(this->data, StringView(tmp.GetIterator(), tmp.GetSentinel()));

        return *this;
    }

    Path Path::operator/(const Path& path) const
    {
        Path copy(GetSize() + path.GetSize(), GetAllocator());
        copy.Append(*this);
        copy.Append(path);

        return copy;
    }

    Path& Path::operator/=(const Path& path)
    {
        Append(path);

        return *this;
    }

    Path& Path::Back()
    {
        Detail::ConcatPaths(this->data, StringView(u8"../"));

        return *this;
    }

    Path Path::operator<<(DeviceSize steps) const
    {
        Path copy(*this);

        copy <<= steps;

        return copy;
    }

    Path& Path::operator<<=(DeviceSize steps)
    {
        for(DeviceSize i = 0; i < steps; i++)
        {
            if(this->data == u8"/")
                break;
            else if(this->data.IsEmpty())
            {
                this->data.Reserve((steps - i) * (sizeof(u8"../") - 1));
                for(; i < steps; i++)
                {
                    Back();
                }

                break;
            }
            else
                Back();
        }

        return *this;
    }

    StringView Path::GetExtension() const noexcept
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

    StringView Path::GetFileName() const noexcept
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

    Bool Path::IsAbsolute() const noexcept
    {
        return this->data.StartsWith(u8"/");
    }

    Bool Path::IsEmpty() const noexcept
    {
        return this->data.IsEmpty();
    }

    DeviceSize Path::GetSize() const noexcept
    {
        return this->data.GetSize();
    }

    DeviceSize Path::GetCapacity() const noexcept
    {
        return this->data.GetCapacity();
    }

    Allocator Path::GetAllocator() const noexcept
    {
        return this->data.GetAllocator();
    }

    StringView Path::GetData() const noexcept
    {
        return StringView(this->data.GetIterator(), this->data.GetSentinel());
    }

    Void Path::Reserve(DeviceSize reserve)
    {
        this->data.Reserve(reserve);
    }

    Void Path::Clear() noexcept
    {
        return this->data.Clear();
    }

    Bool Path::FlushUnusedReserve() noexcept
    {
        return this->data.FlushUnusedReserve();
    }

    Path::Iterator Path::GetIterator() const noexcept
    {
        return Detail::PathPartIterator(StringView(this->data.GetIterator(), this->data.GetSentinel()), this->data.GetIterator());
    }

    Path::Iterator Path::GetSentinel() const noexcept
    {
        return Detail::PathPartIterator(StringView(this->data.GetIterator(), this->data.GetSentinel()), this->data.GetSentinel());
    }

    MemoryRequirements Path::GetMemoryRequirements(DeviceSize reserve) noexcept
    {
        return String::GetMemoryRequirements(reserve);
    }
};