#pragma once

#include <cassert>

namespace Core
{
    constexpr static size_t UUID_SIZE = 16;

    class alignas(UUID_SIZE) UUID
    {
    private:
        consteval unsigned char HexToDec(char value) noexcept
        {
            if(value >= '0' && value <= '9')
                return value - '0';

            return (value - 'a') + 10;
        }
    public:
        consteval UUID() noexcept
            : data{}
        {}

        consteval UUID(const unsigned char (&raw)[UUID_SIZE]) noexcept
        {
            for(size_t i = 0; i < UUID_SIZE; i++)
                this->data[i] = raw[i];
        }

        //123e4567-e89b-12d3-a456-426655440000
        //8-4-4-4-12
        consteval UUID(const char (&data)[37]) noexcept
        {
            int index = 0;
            size_t data_index = 0;
            for(size_t i = 0; i < 36; i++)
            {
                if(i == 8 || i == 13 || i == 18 || i == 23)
                {
                    assert(data[i] == '-');
                    index = 0;
                }
                else
                {
                    assert((data[i] >= '0' && data[i] <= '9') || (data[i] >= 'a' && data[i] <= 'f'));
                    index++;

                    if(index % 2 == 0)
                    {
                        this->data[data_index] = (HexToDec(data[i - 1]) << 4) + HexToDec(data[i]);
                        data_index++;
                    }
                }
            }
        }

        UUID(const UUID&) = default;
        UUID(UUID&&) = default;

        UUID& operator=(const UUID&) = default;
        UUID& operator=(UUID&&) = default;

        bool operator==(const UUID&) const = default;

        constexpr bool operator==(const unsigned char (&raw)[UUID_SIZE]) const noexcept
        {
            for(size_t i = 0; i < UUID_SIZE; i++)
            {
                if(this->data[i] != raw[i])
                    return false;
            }

            return true;
        }
    public:
        unsigned char data[UUID_SIZE];
    };
};