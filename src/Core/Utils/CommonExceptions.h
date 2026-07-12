#pragma once

#include "Exception.h"
#include "String.h"

namespace Core
{
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

    class CORE_API StringEncoderBadCharacterException : public Exception
    {
    public:
        StringEncoderBadCharacterException(DeviceSize input_size, DeviceSize input_offset);

        virtual ~StringEncoderBadCharacterException() override;

        StringEncoderBadCharacterException(const StringEncoderBadCharacterException&) = default;
        StringEncoderBadCharacterException(StringEncoderBadCharacterException&&) = default;
        StringEncoderBadCharacterException& operator=(const StringEncoderBadCharacterException&) = default;
        StringEncoderBadCharacterException& operator=(StringEncoderBadCharacterException&&) = default;

        virtual StringView GetMessage() const noexcept override;
    private:
        DeviceSize input_size;
        DeviceSize input_offset;
        mutable String lazy_message;
    };

    enum class AllocationExceptionType
    {
        OutOfMemory,
        BadMemoryRequirements
    };

    class CORE_API AllocationException : public Exception
    {
    public:
        AllocationException(const MemoryRequirements& req, AllocationExceptionType type);

        virtual ~AllocationException() override;

        AllocationException(const AllocationException&) = default;
        AllocationException(AllocationException&&) = default;
        AllocationException& operator=(const AllocationException&) = default;
        AllocationException& operator=(AllocationException&&) = default;

        virtual StringView GetMessage() const noexcept override;
    private:
        MemoryRequirements req;
        AllocationExceptionType type;
        mutable String lazy_message;
    };
};