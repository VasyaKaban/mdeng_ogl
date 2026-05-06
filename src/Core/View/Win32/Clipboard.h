#pragma once

#include "Core/Utils/NonCreatable.hpp"
#include "../Clipboard.h"

namespace Core
{
    namespace Win32
    {
        class WindowSubsystem;

        class CORE_API Clipboard final : public Core::Clipboard, Core::NonCopyable, Core::NonMovable
        {
        public:
            Clipboard(WindowSubsystem* _parent);
            virtual ~Clipboard() override;

            virtual ClipboardDataType GetDataType() const override;
            virtual std::string_view GetMIMEType() const override;
            virtual std::span<const std::byte> GetData() const override;

            virtual Core::WindowSubsystem* GetParent() const noexcept override;
        private:
            WindowSubsystem* parent;
            HANDLE handle;
            LPVOID raw_data;
            UINT format;
            ClipboardDataType data_type;
            mutable std::string
                data; //optimize for clipboard text data. mutable due to the lazy nature of GetData() method
        };
    };
};