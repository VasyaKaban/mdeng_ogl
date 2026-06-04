#pragma once

#include "Win32Clipboard.h"

namespace Core
{
    Win32Clipboard::Win32Clipboard(Win32WindowSubsystemConnection* _parent)
        : parent(_parent),
          handle(nullptr),
          raw_data(nullptr),
          format(0),
          data_type(ClipboardDataType::Unknown),
          data()
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
            this->handle = ::GetClipboardData(CF_UNICODETEXT);
            if(this->handle == nullptr)
            {
                CloseClipboard();
                Core::System::ThrowLastError();
            }

            this->raw_data = GlobalLock(this->handle);

            this->format = CF_UNICODETEXT;
            this->data_type = ClipboardDataType::MIME;
        }
        else
            CloseClipboard();
    }

    Win32Clipboard::~Win32Clipboard()
    {
        if(this->handle != nullptr)
        {
            GlobalUnlock(this->handle);
            CloseClipboard();
        }
    }

    ClipboardDataType Win32Clipboard::GetDataType() const
    {
        return this->data_type;
    }

    std::string_view Win32Clipboard::GetMIMEType() const
    {
        if(this->data_type != Core::ClipboardDataType::MIME)
            return {};

        if(this->format == CF_UNICODETEXT)
            return "text/plain";

        return {};
    }

    std::span<const std::byte> Win32Clipboard::GetData() const
    {
        //LAZY
        if(this->data.empty())
        {
            if(this->format == CF_UNICODETEXT)
            {
                auto wide_string_size = wcsnlen(static_cast<const wchar_t*>(this->raw_data),
                                                GlobalSize(this->handle) / sizeof(wchar_t));

                std::wstring_view wstr(static_cast<const wchar_t*>(this->raw_data),
                                       wide_string_size);

                this->data = Core::System::WideToUTF8(wstr);
            }
        }

        return {reinterpret_cast<const std::byte*>(this->data.data()), this->data.size()};
    }

    WindowSubsystemConnection* Win32Clipboard::GetParent() const noexcept
    {
        return this->parent;
    }
};