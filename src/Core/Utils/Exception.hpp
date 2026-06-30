#pragma once

#include "StringView.hpp"
#include "String"

namespace Core
{
    class CORE_API Exception
    {
    public:
        virtual ~Exception() = 0;

        virtual StringView GetMessage() const noexcept = 0;
    };

    class CORE_API RuntimeException : public Exception
    {
    public:
        RuntimeException() noexcept = default;
        RuntimeException(const String& message);
        RuntimeException(String&& message) noexcept;

        virtual ~RuntimeException() override;

        RuntimeException(const RuntimeException&) = default;
        RuntimeException(RuntimeException&&) = default;
        RuntimeException& operator=(const RuntimeException&) = default;
        RuntimeException& operator=(RuntimeException&&) = default;

        virtual StringView GetMessage() const noexcept override;
    private:
        String message;
    };
};