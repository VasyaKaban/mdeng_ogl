#include "Exception.hpp"

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
};