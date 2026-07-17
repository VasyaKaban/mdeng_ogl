#pragma once

#include "Traits.hpp"

namespace Core
{
    /*
    FMA: a * b + c
    Init: a, b, c
    Generate():
        res = a * b + c
        a = b
        b = c
        c = res

        return res
    
    */
    template<UnsignedIntegral I>
    class FMARandomGenerator
    {
    public:
        constexpr FMARandomGenerator(I a, I b, I c) noexcept
            : a(a),
              b(b),
              c(c)
        {
            //skip first N samples for case when a, c and c are so small values that first calls will give us strictly increasing sequence(ex: a = 0, b = 0, c = 1)
            for(Int32 i = 0; i < 16; i++)
                (*this)();
        }

        ~FMARandomGenerator() = default;
        FMARandomGenerator(const FMARandomGenerator&) = default;
        FMARandomGenerator(FMARandomGenerator&&) = default;
        FMARandomGenerator& operator=(const FMARandomGenerator&) = default;
        FMARandomGenerator& operator=(FMARandomGenerator&&) = default;

        constexpr I operator()() noexcept
        {
            I ret = this->a * this->b + this->c;
            this->a = this->b;
            this->b = this->c;
            this->c = ret;

            return ret;
        }

        constexpr I GetA() const noexcept
        {
            return this->a;
        }

        constexpr I GetB() const noexcept
        {
            return this->b;
        }

        constexpr I GetC() const noexcept
        {
            return this->c;
        }
    private:
        I a;
        I b;
        I c;
    };
};