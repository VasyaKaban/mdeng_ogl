#pragma once

#include <cassert>
#include "Traits.hpp"
#include "Types.hpp"

namespace Core
{

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    requires SameAs<DropConstVolatileReference<I>, I> && Arithmetic<I> && (Size > 0)
    class Vector
    {
    public:
        using Iterator = I*;
        using ConstIterator = const I*;

        Vector() = default;

        constexpr Vector(I fill_value) noexcept
        {
            for(DeviceSize i = 0; i < Size; i++)
                this->data[i] = fill_value;
        }

        ~Vector() = default;
        Vector(const Vector&) = default;
        Vector(Vector&&) = default;
        Vector& operator=(const Vector&) = default;
        Vector& operator=(Vector&&) = default;

        template<typename... Args>
        requires(sizeof...(Args) <= Size) && (Arithmetic<Args> && ...)
        constexpr Vector(Args... args) noexcept
            : data{static_cast<I>(args)...}
        {}

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector(const Vector<OI, OtherAlignment, OtherSize>& vec) noexcept
        {
            DeviceSize size = Min(Size, OtherSize);
            for(DeviceSize i = 0; i < size; i++)
                this->data[i] = vec[i];
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector(const Vector<OI, OtherAlignment, OtherSize>& vec, I fill_value) noexcept
            : Vector(vec)
        {
            DeviceSize size = Min(Size, OtherSize);

            for(DeviceSize i = size; i < Size; i++)
                this->data[i] = fill_value;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector& operator=(const Vector<OI, OtherAlignment, OtherSize>& vec) noexcept
        {
            DeviceSize size = Min(Size, OtherSize);
            for(DeviceSize i = 0; i < size; i++)
                this->data[i] = vec[i];

            return *this;
        }

        constexpr I* GetData() noexcept
        {
            return this->data;
        }

        constexpr const I* GetData() const noexcept
        {
            return this->data;
        }

        constexpr I& operator[](DeviceSize index) noexcept
        {
            assert(index < Size);

            return this->data[index];
        }

        constexpr const I& operator[](DeviceSize index) const noexcept
        {
            assert(index < Size);

            return this->data[index];
        }

        constexpr Iterator GetIterator() noexcept
        {
            return Iterator(this->data);
        }

        constexpr ConstIterator GetIterator() const noexcept
        {
            return ConstIterator(this->data);
        }

        constexpr Iterator GetSentinel() noexcept
        {
            return Iterator(this->data + Size);
        }

        constexpr ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(this->data + Size);
        }

        template<Arithmetic T>
        constexpr Vector operator+(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp += value;

            return out;
        }

        template<Arithmetic T>
        constexpr Vector& operator+=(T value) noexcept
        {
            for(auto& comp: *this)
                comp += value;

            return *this;
        }

        template<Arithmetic T>
        constexpr Vector operator-(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp -= value;

            return out;
        }

        template<Arithmetic T>
        constexpr Vector& operator-=(T value) noexcept
        {
            for(auto& comp: *this)
                comp -= value;

            return *this;
        }

        template<Arithmetic T>
        constexpr Vector operator*(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp *= value;

            return out;
        }

        template<Arithmetic T>
        constexpr Vector& operator*=(T value) noexcept
        {
            for(auto& comp: *this)
                comp *= value;

            return *this;
        }

        template<Arithmetic T>
        constexpr Vector operator/(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp /= value;

            return out;
        }

        template<Arithmetic T>
        constexpr Vector& operator/=(T value) noexcept
        {
            for(auto& comp: *this)
                comp /= value;

            return *this;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector operator+(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            Vector result(*this);
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                result[i] += vec.data[i];

            return result;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector& operator+=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                this->data[i] += vec[i];

            return *this;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector operator-(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            Vector result(*this);
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                result[i] -= vec.data[i];

            return result;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector& operator-=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                this->data[i] -= vec[i];

            return *this;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector operator*(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            Vector result(*this);
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                result[i] *= vec.data[i];

            return result;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector& operator*=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                this->data[i] *= vec[i];

            return *this;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector operator/(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            Vector result(*this);
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                result[i] /= vec.data[i];

            return result;
        }

        template<typename OI, DeviceSize OtherAlignment, DeviceSize OtherSize>
        constexpr Vector& operator/=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(DeviceSize i = 0; i < Min(Size, OtherSize); i++)
                this->data[i] /= vec[i];

            return *this;
        }

        Bool operator==(const Vector& vec) const noexcept;

        constexpr auto GetLength() const noexcept
        {
            using OutType = CommonArithmetic<I, Float32>;

            OutType result = 0;
            for(const I comp: *this)
                result += comp * comp;

            return sqrtf(result);
        }

        constexpr auto GetInvertedLength() const noexcept
        {
            return decltype(GetLength())(1.0) / GetLength();
        }

        constexpr Vector Normalized() const noexcept
        {
            Vector out(*this);

            auto inv_len = GetInvertedLength();

            for(auto& comp: out)
                comp *= inv_len;

            return out;
        }

        constexpr Vector& Normalize() noexcept
        {
            auto inv_len = GetInvertedLength();

            for(auto& comp: &this)
                comp *= inv_len;

            return *this;
        }

        template<typename OI, DeviceSize OtherAlignment>
        constexpr auto Dot(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            using OutType = CommonArithmetic<I, OI, Float32>;

            OutType result = 0;
            for(DeviceSize i = 0; i < Size; i++)
                result += this->data[i] * vec.data[i];

            return result;
        }

        template<typename OI, DeviceSize OtherAlignment>
        constexpr auto Cos(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            using OutType = CommonArithmetic<I, OI, Float32>;

            OutType result = 0;
            for(DeviceSize i = 0; i < Size; i++)
                result += this->data[i] * vec.data[i];

            result *= GetInvertedLength();
            result *= vec.GetInvertedLength();

            return result;
        }

        //use it when we know that both vectors are normalized
        template<typename OI, DeviceSize OtherAlignment>
        constexpr auto RawCos(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            return Dot(vec);
        }

        template<typename OI, DeviceSize OtherAlignment>
        constexpr Vector Project(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            using OutType = CommonArithmetic<I, OI, Float32>;

            OutType result = 0;
            for(DeviceSize i = 0; i < Size; i++)
                result += this->data[i] * vec.data[i];

            result *= vec.GetInvertedLength();

            return Vector(vec) * result;
        }

        template<typename OI, DeviceSize OtherAlignment>
        requires(Size == 3)
        constexpr Vector Cross(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            //x = v0y * v1z - v1y * v0z;
            //y = -(v0x * v1z - v1x * v0z);
            //z = v0x * v1y - v1x * v0y

            return Vector(this->data[1] * vec[2] - vec[1] * this->data[2], -(this->data[0] * vec[2] - vec[0] * this->data[2]), this->data[0] * vec[1] - vec[0] * this->data[1]);
        }

        constexpr Vector operator-() const noexcept
        {
            Vector out(*this);
            for(auto& comp: out)
                comp = -comp;

            return out;
        }

        constexpr Vector& InverseSign() noexcept
        {
            for(auto& comp: *this)
                comp = -comp;

            return *this;
        }

        consteval static DeviceSize GetSize() noexcept
        {
            return Size;
        }
    private:
        alignas(Alignment) I data[Size];
    };

    template<typename I>
    Vector(I fill_value) -> Vector<I, alignof(I), 1>;

    template<typename... Args>
    Vector(Args...) -> Vector<CommonArithmetic<Args...>, alignof(CommonArithmetic<Args...>), sizeof...(Args)>;

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    Vector(const Vector<I, Alignment, Size>&) -> Vector<I, Alignment, Size>;

    //std compat
    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto begin(const Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetIterator();
    }

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto begin(Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetIterator();
    }

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto begin(Vector<I, Alignment, Size>&& vec) noexcept
    {
        return Move(vec).GetIterator();
    }

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto end(const Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetSentinel();
    }

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto end(Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetSentinel();
    }

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto end(Vector<I, Alignment, Size>&& vec) noexcept
    {
        return Move(vec).GetSentinel();
    }

    template<typename I, DeviceSize Alignment, DeviceSize Size>
    constexpr auto size(const Vector<I, Alignment, Size>& vec) noexcept
    {
        return Size;
    }

    namespace GLSL
    {
        namespace Scalar
        {
            using Vector1i32 = Vector<Int32, alignof(Int32), 1>;
            using Vector2i32 = Vector<Int32, alignof(Int32), 2>;
            using Vector3i32 = Vector<Int32, alignof(Int32), 3>;
            using Vector4i32 = Vector<Int32, alignof(Int32), 4>;

            using Vector1ui32 = Vector<UInt32, alignof(UInt32), 1>;
            using Vector2ui32 = Vector<UInt32, alignof(UInt32), 2>;
            using Vector3ui32 = Vector<UInt32, alignof(UInt32), 3>;
            using Vector4ui32 = Vector<UInt32, alignof(UInt32), 4>;

            using Vector1f32 = Vector<Float32, alignof(Float32), 1>;
            using Vector2f32 = Vector<Float32, alignof(Float32), 2>;
            using Vector3f32 = Vector<Float32, alignof(Float32), 3>;
            using Vector4f32 = Vector<Float32, alignof(Float32), 4>;

            using Vector1f64 = Vector<Float64, alignof(Float64), 1>;
            using Vector2f64 = Vector<Float64, alignof(Float64), 2>;
            using Vector3f64 = Vector<Float64, alignof(Float64), 3>;
            using Vector4f64 = Vector<Float64, alignof(Float64), 4>;

            using Vector1b32 = Vector1i32;
            using Vector2b32 = Vector2i32;
            using Vector3b32 = Vector3i32;
            using Vector4b32 = Vector4i32;
        };

        namespace STD140
        {
            using Vector1i32 = Vector<Int32, alignof(Int32), 1>;
            using Vector2i32 = Vector<Int32, alignof(Int32) * 2, 2>;
            using Vector3i32 = Vector<Int32, alignof(Int32) * 4, 3>;
            using Vector4i32 = Vector<Int32, alignof(Int32) * 4, 4>;

            using Vector1ui32 = Vector<UInt32, alignof(UInt32), 1>;
            using Vector2ui32 = Vector<UInt32, alignof(UInt32) * 2, 2>;
            using Vector3ui32 = Vector<UInt32, alignof(UInt32) * 4, 3>;
            using Vector4ui32 = Vector<UInt32, alignof(UInt32) * 4, 4>;

            using Vector1f32 = Vector<Float32, alignof(Float32), 1>;
            using Vector2f32 = Vector<Float32, alignof(Float32) * 2, 2>;
            using Vector3f32 = Vector<Float32, alignof(Float32) * 4, 3>;
            using Vector4f32 = Vector<Float32, alignof(Float32) * 4, 4>;

            using Vector1f64 = Vector<Float64, alignof(Float64), 1>;
            using Vector2f64 = Vector<Float64, alignof(Float64) * 2, 2>;
            using Vector3f64 = Vector<Float64, alignof(Float64) * 4, 3>;
            using Vector4f64 = Vector<Float64, alignof(Float64) * 4, 4>;

            using Vector1b32 = Vector1i32;
            using Vector2b32 = Vector2i32;
            using Vector3b32 = Vector3i32;
            using Vector4b32 = Vector4i32;
        };

        namespace STD430
        {
            using Vector1i32 = Vector<Int32, alignof(Int32), 1>;
            using Vector2i32 = Vector<Int32, alignof(Int32) * 2, 2>;
            using Vector3i32 = Vector<Int32, alignof(Int32) * 4, 3>;
            using Vector4i32 = Vector<Int32, alignof(Int32) * 4, 4>;

            using Vector1ui32 = Vector<UInt32, alignof(UInt32), 1>;
            using Vector2ui32 = Vector<UInt32, alignof(UInt32) * 2, 2>;
            using Vector3ui32 = Vector<UInt32, alignof(UInt32) * 4, 3>;
            using Vector4ui32 = Vector<UInt32, alignof(UInt32) * 4, 4>;

            using Vector1f32 = Vector<Float32, alignof(Float32), 1>;
            using Vector2f32 = Vector<Float32, alignof(Float32) * 2, 2>;
            using Vector3f32 = Vector<Float32, alignof(Float32) * 4, 3>;
            using Vector4f32 = Vector<Float32, alignof(Float32) * 4, 4>;

            using Vector1f64 = Vector<Float64, alignof(Float64), 1>;
            using Vector2f64 = Vector<Float64, alignof(Float64) * 2, 2>;
            using Vector3f64 = Vector<Float64, alignof(Float64) * 4, 3>;
            using Vector4f64 = Vector<Float64, alignof(Float64) * 4, 4>;

            using Vector1b32 = Vector1i32;
            using Vector2b32 = Vector2i32;
            using Vector3b32 = Vector3i32;
            using Vector4b32 = Vector4i32;
        };
    };
};