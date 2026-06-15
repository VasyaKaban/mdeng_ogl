#pragma once

#include "Nullable.hpp"

namespace Core
{
    template<typename F>
    requires std::invocable<F>
    class ScopedCall : private Nullable<F>
    {
    public:
        using Nullable<F>::Nullable;
        using Nullable<F>::operator=;
        using Nullable<F>::Clear;

        ~ScopedCall()
        {
            if(this->HasValue())
                this->GetValue()();
        }

        void Call() const noexcept(std::is_nothrow_invocable_v<F>)
        {
            if(this->HasValue())
                this->GetValue()();
        }
    };

    template<typename F>
    requires std::invocable<F>
    ScopedCall(F&&) -> ScopedCall<std::remove_reference_t<F>>;
};