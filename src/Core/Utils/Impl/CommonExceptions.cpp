#include "../CommonExceptions.h"
#include "../StringView.h"
#include "../Format.hpp"

namespace Core
{
    Exception::~Exception()
    {}

    RuntimeException::RuntimeException(const String& message)
        : message(message)
    {}

    RuntimeException::RuntimeException(String&& message) noexcept
        : message(Move(message))
    {}

    RuntimeException::~RuntimeException()
    {}

    StringView RuntimeException::GetMessage() const noexcept
    {
        return StringView(this->message);
    }

    StringEncoderBadCharacterException::StringEncoderBadCharacterException(DeviceSize input_size, DeviceSize input_offset)
        : input_size(input_size),
          input_offset(input_offset)
    {}

    StringEncoderBadCharacterException::~StringEncoderBadCharacterException()
    {}

    StringView StringEncoderBadCharacterException::GetMessage() const noexcept
    {
        if(this->lazy_message.IsEmpty())
            this->lazy_message = Format("Bad character during UTF8 conversion. Input size: ", this->input_size, "; Input offset: ", this->input_offset);

        return StringView(this->lazy_message);
    }

    AllocationException::AllocationException(const MemoryRequirements& req, AllocationExceptionType type)
        : req(req),
          type(type)
    {}

    AllocationException::~AllocationException()
    {}

    StringView AllocationException::GetMessage() const noexcept
    {
        if(this->lazy_message.IsEmpty())
        {
            StringView type_msg;
            switch(this->type)
            {
                case AllocationExceptionType::OutOfMemory:
                    type_msg = u8"Out of memory.";
                    break;
                case AllocationExceptionType::BadMemoryRequirements:
                    type_msg = u8"Bad memory requirements.";
                    break;
            }

            this->lazy_message = Format(type_msg, " Alignment: ", this->req.alignment, "; Size: ", this->req.size);
        }

        return StringView(this->lazy_message);
    }

};