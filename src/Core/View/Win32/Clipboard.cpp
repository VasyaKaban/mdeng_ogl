#pragma once

#include "Clipboard.h"
#include "WindowSubsystem.h"
#include "Core/Utils/System.h"
#include "Core/Utils/Unicode.h"

namespace Core
{
    namespace Win32
    {
        //WindowSubsystem* parent;
        // HANDLE handle;
        //   UINT format;
        //  ClipboardDataType data_type;
        // std::span<const std::byte> data;
        Clipboard::Clipboard(WindowSubsystem* _parent)
            : parent(_parent),
              handle(nullptr),
              raw_data(nullptr),
              format(0),
              data_type(ClipboardDataType::Unknown),
              data({})
        {
            if(OpenClipboard(nullptr) == 0)
                Core::System::ThrowLastError();

            BOOL res = IsClipboardFormatAvailable(CF_UNICODETEXT);
            if(res == 0)
            {
                CloseClipboard();
                Core::System::ThrowLastError();
            }

            if(res == TRUE)
            {
                handle = ::GetClipboardData(CF_UNICODETEXT);
                if(handle == nullptr)
                {
                    CloseClipboard();
                    Core::System::ThrowLastError();
                }

                raw_data = GlobalLock(handle);

                format = CF_UNICODETEXT;
                data_type = ClipboardDataType::MIME;
            }
            else
                CloseClipboard();
        }

        Clipboard::~Clipboard()
        {
            if(handle != nullptr)
            {
                GlobalUnlock(handle);
                CloseClipboard();
            }
        }

        ClipboardDataType Clipboard::GetDataType() const
        {
            return data_type;
        }

        std::string_view Clipboard::GetMIMEType() const
        {
            if(data_type != Core::ClipboardDataType::MIME)
                return {};

            if(format == CF_UNICODETEXT)
                return "text/plain";

            return {};
        }

        std::span<const std::byte> Clipboard::GetData() const
        {
            //LAZY
            if(data.empty())
            {
                if(format == CF_UNICODETEXT)
                {
                    auto wide_string_size = wcsnlen(static_cast<const wchar_t*>(raw_data),
                                                    GlobalSize(handle) / sizeof(wchar_t));

                    data = Core::UTF16ToUTF8(
                        {static_cast<const char16_t*>(raw_data), wide_string_size});
                }
            }

            return {reinterpret_cast<const std::byte*>(data.data()), data.size()};
        }

        Core::WindowSubsystem* Clipboard::GetParent() const noexcept
        {
            return parent;
        }
    };
};