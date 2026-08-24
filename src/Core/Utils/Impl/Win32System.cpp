#include "../Platform.h"

#if CORE_PLATFORM_CURRENT == CORE_PLATFORM_WIN32

#    include "Win32System.h"
#    include "../Sequence.hpp"

namespace Core
{

    Win32Exception::Win32Exception(DWORD error)
        : error(error)
    {}

    Win32Exception::~Win32Exception()
    {}

    StringView Win32Exception::GetMessage() const noexcept
    {
        if(this->lazy_message.IsEmpty())
        {
            LPWSTR msg_str = nullptr;
            auto msg_size =
                FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, this->error, 0, reinterpret_cast<LPWSTR>(&msg_str), 0, nullptr);

            this->lazy_message.Append(msg_str, msg_size);

            LocalFree(msg_str);
        }

        return this->lazy_message;
    }

    struct Win32SystemData
    {
        Int32 cmd_show;
        DWORD main_thread_id;
        Path executable_path;
        DeviceSize concurrent_share_granularity_alignment;
        DeviceSize concurrent_share_granularity_size;
    };

    static Win32SystemData Win32SystemDataInstance = {};

    Void Win32System::Init(const Win32SystemInitInfo& info)
    {
        Win32SystemDataInstance.cmd_show = info.cmd_show;
        Win32SystemDataInstance.main_thread_id = info.main_thread_id;

        Sequence<wchar_t> name;
        name.Resize(MAX_PATH);
        DWORD size = 0;

        while(true)
        {
            SetLastError(ERROR_SUCCESS);
            size = GetModuleFileNameW(nullptr, name.GetData(), name.GetSize());

            auto last_error = GetLastError();
            if(last_error == ERROR_INSUFFICIENT_BUFFER)
                name.Resize(name.GetSize() * 2);
            else if(last_error == ERROR_SUCCESS)
                break;
            else
                throw Win32Exception(last_error);
        }

        TranslateFromWin32PathToAbsolutePath(name.GetData(), size, Win32SystemDataInstance.executable_path);
        Win32SystemDataInstance.executable_path.Back(); //erase filename

#    if CORE_ARCH_CURRENT == CORE_ARCH_AMD64
        Win32SystemDataInstance.concurrent_share_granularity_alignment = 128; //let's use 128 bytes instead of 64 to cover Intel's spatial prefetcher
        Win32SystemDataInstance.concurrent_share_granularity_size = 64; //let's use 64 bytes because it's a min value for L1 cache line size
#    else
#        error "Unsupported arch"
#    endif

        DWORD cache_buffer_size = 0;
        if(GetLogicalProcessorInformationEx(RelationCache, nullptr, &cache_buffer_size) != TRUE)
            throw Win32Exception(GetLastError());

        Sequence<UInt8> cache_buffer;
        cache_buffer.Resize(cache_buffer_size);

        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX cache_info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(cache_buffer.GetData());
        if(GetLogicalProcessorInformationEx(LOGICAL_PROCESSOR_RELATIONSHIP::RelationCache, cache_info, &cache_buffer_size) != TRUE)
            throw Win32Exception(GetLastError());

        DWORD cache_buffer_offset = 0;
        while(cache_buffer_offset < cache_buffer_size)
        {
            cache_info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(cache_buffer.GetData() + cache_buffer_offset);

            if(cache_info->Relationship == RelationCache)
            {
                if(cache_info->Cache.Level == 1 && cache_info->Cache.Type == PROCESSOR_CACHE_TYPE::CacheData)
                {
                    if(Win32SystemDataInstance.concurrent_share_granularity_alignment < cache_info->Cache.LineSize)
                        Win32SystemDataInstance.concurrent_share_granularity_alignment = cache_info->Cache.LineSize;

                    if(Win32SystemDataInstance.concurrent_share_granularity_size > cache_info->Cache.LineSize)
                        Win32SystemDataInstance.concurrent_share_granularity_size = cache_info->Cache.LineSize;
                }
            }

            cache_buffer_offset += cache_info->Size;
        }
    }

    Void Win32System::GetRandomBytes(UInt8* output, DeviceSize size)
    {
        NTSTATUS status = BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(output), size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if(status != STATUS_SUCCESS)
            throw Win32Exception(LsaNtStatusToWinError(status));
    }

    const Path& Win32System::GetExecutablePath()
    {
        return Win32SystemDataInstance.executable_path;
    }

    String Win32System::DecorateDynamicLibraryName(StringView name)
    {
        String res(name.GetSize() + 4); //reserve 4 bytes for '.dll'

        res.Append(name);
        res.Append(".dll");

        return res;
    }

    String Win32System::UndecorateSymbol(StringView name)
    {
        if(!name.StartsWith(u8"?"))
            return name;

        auto utf16_size = StringEncoder::GetWideCharSize(name.GetData(), name.GetSize());

        Sequence<wchar_t> utf16_symbol_name(utf16_size + 1);

        StringEncoder::ConvertToWideChar(name.GetData(), name.GetSize(), utf16_symbol_name.GetData());
        utf16_symbol_name.GetData()[utf16_size] = L'\0';

        wchar_t undecorated_string[MAX_SYM_NAME];

        DWORD res = UnDecorateSymbolNameW(utf16_symbol_name.GetData(), undecorated_string, MAX_SYM_NAME, UNDNAME_COMPLETE);
        if(res == 0)
            throw Win32Exception(GetLastError());

        return String(undecorated_string, res);
    }

    DeviceSize Win32System::GetConcurrentShareGranularityAlignment() noexcept
    {
        return Win32SystemDataInstance.concurrent_share_granularity_alignment;
    }

    DeviceSize Win32System::GetConcurrentShareGranularitySize() noexcept
    {
        return Win32SystemDataInstance.concurrent_share_granularity_size;
    }

    Sequence<WideChar>* Win32System::GetThreadLocalWideCharBuffer() noexcept
    {
        thread_local Sequence<WideChar> buffer;

        return &buffer;
    }

    Void Win32System::TranslateAbsolutePathToWin32Path(const PathView& path, Sequence<WideChar>& output)
    {
        assert(path.IsAbsolute());

        auto ptr = (++path.GetData().GetIterator()).GetAddress(); //skip POSIX root

        DeviceSize output_size = AbsolutePathImplementationReserve /*prefix + null-term*/ + StringEncoder::GetUTF16Size(ptr, path.GetSize() - 1); //path without POSIX root
        output.Resize(output_size);

        //prefix
        CopyNonOverlappedMemory(AbsolutePathPrefix, output.GetData(), AbsolutePathPrefixSize * sizeof(WideChar));

        WideChar* post_prefix_output = output.GetData() + AbsolutePathPrefixSize;

        DeviceSize null_term_pos = StringEncoder::ConvertToWideChar(ptr, path.GetSize() - 1, post_prefix_output);

        post_prefix_output[null_term_pos] = L'\0';
    }

    Void Win32System::TranslateFromWin32PathToAbsolutePath(WideChar* input, DeviceSize input_size, Path& output) //changes \ to / and removes prefix
    {
        //erase prefix
        if(input_size >= 4)
        {
            Bool has_prefix = true;
            for(DeviceSize i = 0; i < AbsolutePathPrefixSize; i++)
            {
                if(input[i] != AbsolutePathPrefix[i])
                {
                    has_prefix = false;
                    break;
                }
            }

            if(has_prefix)
            {
                input += 4;
                input_size -= 4;
            }
        }

        //change '\' to '/'
        for(DeviceSize i = 0; i < input_size; i++)
        {
            if(input[i] == L'\\')
                input[i] = L'/';
        }

        auto res = StringEncoder::GetLength(input, input_size);

        output.Clear();
        output.Reserve(res.output_size + 1); //+1 for root

        output.Append(u8"/");
        output.Append(input, input_size);
    }
};

#endif