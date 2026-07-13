#include "../Path.h"
#include "../Types.hpp"
#include "../StringView.h"

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

        using ConcatFunction = DeviceSize /*new size*/ (*)(UTF8Char* current_str, DeviceSize current_str_size, const UTF8Char* input_str, DeviceSize input_str_size);

        DeviceSize PathMatrixAppendToPath(UTF8Char* current_str, DeviceSize current_str_size, const UTF8Char* input_str, DeviceSize input_str_size)
        {
            current_str[current_str_size] = u8'/';
            CopyNonOverlappedMemory(input_str, current_str + current_str_size + 1, input_str_size);

            return current_str_size + 1 + input_str_size;
        };

        DeviceSize PathMatrixNoOp(UTF8Char* current_str, DeviceSize current_str_size, const UTF8Char* input_str, DeviceSize input_str_size)
        {
            return current_str_size;
        };

        DeviceSize PathMatrixCopyContent(UTF8Char* current_str, DeviceSize current_str_size, const UTF8Char* input_str, DeviceSize input_str_size)
        {
            CopyNonOverlappedMemory(input_str, current_str + current_str_size, input_str_size);

            return current_str_size + input_str_size;
        };

        DeviceSize PathMatrixAssert(UTF8Char* current_str, DeviceSize current_str_size, const UTF8Char* input_str, DeviceSize input_str_size)
        {
            assert(false);

            return current_str_size;
        };

        DeviceSize PathMatrixBacktrace(UTF8Char* current_str, DeviceSize current_str_size, const UTF8Char* input_str, DeviceSize input_str_size)
        {
            auto slash_ptr = Core::Detail::FindInStringReverse(current_str, current_str_size, u8"/", 1);
            if(slash_ptr == nullptr) //no slash -> empty
                return 0;
            else
                return slash_ptr - current_str;
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
        static void ConcatPaths(StringView current, StringView input)
        {
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
        }

        /*
def AddPath(app_path: str) -> str:
    while len(app_path) != 0:
        token = ""
        token_index = EMPTY_INDEX
        if app_path.startswith("/"):
            token_index = ROOT_INDEX
            token = "/"
            app_path = SkipSlashes(app_path)
        elif app_path.startswith("."):
            if app_path.startswith("./"):
                token_index = CURRENT_INDEX
                token = "."
                app_path = app_path[2:]
                app_path = SkipSlashes(app_path)
            elif app_path == ".":
                token_index = CURRENT_INDEX
                token = "."
                app_path = ""
            elif app_path.startswith("../"):
                token_index = BACK_INDEX
                token = ".."
                app_path = app_path[3:]
                app_path = SkipSlashes(app_path)
            elif app_path == "..":
                token_index = BACK_INDEX
                token = ".."
                app_path = ""
            else:
                slash_index = app_path.find("/")
                if slash_index == -1:
                    token_index = FILE_INDEX
                    token = f"{app_path}"
                    app_path = ""
                else:
                    token_index = FILE_INDEX
                    token = f"{app_path[0:slash_index]}"
                    app_path = app_path[slash_index:]
                    app_path = SkipSlashes(app_path)
        else:
            slash_index = app_path.find("/")
            if slash_index == -1:
                token_index = FILE_INDEX
                token = f"{app_path}"
                app_path = ""
            else:
                token_index = FILE_INDEX
                token = f"{app_path[0:slash_index]}"
                app_path = app_path[slash_index:]
                app_path = SkipSlashes(app_path)

        result_token_index = EMPTY_INDEX
        if len(result) != 0:
            if result.endswith("/"):
                if result == "/":
                    result_token_index = ROOT_INDEX
            elif result.endswith("/..") or result == "..":
                result_token_index = BACK_INDEX
            elif result.endswith("/.") or result == ".":
                result_token_index = CURRENT_INDEX
            else:
                result_token_index = FILE_INDEX     

        result = CONCAT_MATRIX[result_token_index][token_index](result, token)

    return result*/
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Path::Path(Allocator allocator = GetGlobalAllocator());
    Path::Path(DeviceSize reserve, Allocator allocator = GetGlobalAllocator());
    Path::~Path();
    Path::Path(const Path& path);
    Path::Path(Path&& path) noexcept;
    Path& Path::operator=(const Path& path);
    Path& Path::operator=(Path&& path) noexcept;

    Path::Path(const StringView& str, Allocator allocator = GetGlobalAllocator());
    Path& Path::operator=(StringView str);

    Path::Path(Detail::StringCharIterator<const UTF8Char> begin, Detail::StringCharIterator<const UTF8Char> end, Allocator allocator = GetGlobalAllocator());
    Path::Path(const Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
    Path::Path(const WideChar* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
    Path::Path(const UTF8Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
    Path::Path(const UTF16Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());
    Path::Path(const UTF32Char* input, DeviceSize input_size, Allocator allocator = GetGlobalAllocator());

    Path& Path::Append(const Path& path);
    Path& Path::Append(Iterator begin, Iterator end);

    Path Path::operator/(const Path& path);
    Path& Path::operator/=(const Path& path);

    Path& Path::Back();
    Path Path::operator<<(DeviceSize steps);
    Path& Path::operator<<=(DeviceSize steps);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        return (this->IsEmpty() ? false : this->data.GetData()[0] == u8'/');
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

    Iterator Path::GetIterator() const noexcept
    {
        return Detail::PathPartIterator(StringView(this->data.GetIterator(), this->data.GetSentinel()), this->data.GetIterator());
    }

    Iterator Path::GetSentinel() const noexcept
    {
        return Detail::PathPartIterator(StringView(this->data.GetIterator(), this->data.GetSentinel()), this->data.GetSentinel());
    }

    MemoryRequirements Path::GetMemoryRequirements(DeviceSize reserve) noexcept
    {
        return String::GetMemoryRequirements(reserve);
    }
};
