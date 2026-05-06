#pragma once

#include "Core/API.h"
#include "View.h"

namespace Core
{
    class CORE_API Clipboard
    {
    public:
        virtual ~Clipboard() = 0;

        virtual ClipboardDataType GetDataType() const = 0;
        virtual std::string_view GetMIMEType() const = 0;
        virtual std::span<const std::byte> GetData() const = 0;

        virtual WindowSubsystem* GetParent() const noexcept = 0;
    };
};