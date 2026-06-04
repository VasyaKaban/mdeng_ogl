#pragma once

#include "Core/Utils/System.h"
#include "Core/Utils/NonCreatable.hpp"
#include "../Clipboard.h"
#include "Win32WindowSubsystem.h"

namespace Core
{
    class CORE_API Win32Clipboard final : public Core::Clipboard
    {
    public:
        CORE_NON_COPYABLE(Win32Clipboard)
        CORE_NON_MOVABLE(Win32Clipboard)

        Win32Clipboard(Win32WindowSubsystemConnection* _parent);
        virtual ~Win32Clipboard() override;

        virtual ClipboardDataType GetDataType() const override;
        virtual std::string_view GetMIMEType() const override;
        virtual std::span<const std::byte> GetData() const override;

        virtual WindowSubsystemConnection* GetParent() const noexcept override;
    private:
        Win32WindowSubsystemConnection* parent;
        HANDLE handle;
        LPVOID raw_data;
        UINT format;
        ClipboardDataType data_type;
        mutable std::string
            data; //optimize for clipboard text data. mutable due to the lazy nature of GetData() method
    };

};