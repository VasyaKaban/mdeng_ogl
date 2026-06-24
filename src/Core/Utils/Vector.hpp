#pragma once

#include <concepts>
#include <utility>

namespace Core
{

    template<typename I, size_t Alignment, size_t Size>
    requires std::same_as<std::remove_cvref_t<I>, I> && std::is_arithmetic_v<I> && (Size > 0)
    class Vector
    {
    public:
        using Iterator = I*;
        using ConstIterator = const I*;

        Vector() = default;

        constexpr Vector(I fill_value) noexcept
        {
            for(size_t i = 0; i < Size; i++)
                this->data[i] = fill_value;
        }

        ~Vector() = default;
        Vector(const Vector&) = default;
        Vector(Vector&&) = default;
        Vector& operator=(const Vector&) = default;
        Vector& operator=(Vector&&) = default;

        template<typename... Args>
        requires(sizeof...(Args) == Size) && (std::is_arithmetic_v<Args> && ...)
        constexpr Vector(Args... args) noexcept
            : data{args...}
        {}

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector(const Vector<OI, OtherAlignment, OtherSize>& vec) noexcept
        {
            size_t size = std::min(Size, OtherSize);
            for(size_t i = 0; i < size; i++)
                this->data[i] = vec[i];
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector(const Vector<OI, OtherAlignment, OtherSize>& vec, I fill_value) noexcept
            : Vector(vec)
        {
            size_t size = std::min(Size, OtherSize);

            for(size_t i = size; i < Size; i++)
                this->data[i] = fill_value;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector& operator=(const Vector<OI, OtherAlignment, OtherSize>& vec) noexcept
        {
            size_t size = std::min(Size, OtherSize);
            for(size_t i = 0; i < size; i++)
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

        constexpr I& operator[](size_t index) noexcept
        {
            assert(index < Size);

            return this->data[index];
        }

        constexpr const I& operator[](size_t index) const noexcept
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

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector operator+(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp += value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector& operator+=(T value) noexcept
        {
            for(auto& comp: *this)
                comp += value;

            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector operator-(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp -= value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector& operator-=(T value) noexcept
        {
            for(auto& comp: *this)
                comp -= value;

            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector operator*(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp *= value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector& operator*=(T value) noexcept
        {
            for(auto& comp: *this)
                comp *= value;

            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector operator/(T value) const noexcept
        {
            Vector out(*this);

            for(auto& comp: out)
                comp /= value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Vector& operator/=(T value) noexcept
        {
            for(auto& comp: *this)
                comp /= value;

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr auto operator+(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            using OutType = Vector<std::common_type_t<I, OI>, std::max(Alignment, OtherAlignment), std::max(Size, OtherSize)>;

            size_t size = std::min(Size, OtherSize);

            OutType result(*this);
            for(size_t i = 0; i < size; i++)
                result[i] += vec.data[i];

            if constexpr(OtherSize > Size)
            {
                for(size_t i = size; i < OtherSize; i++)
                    result[i] = vec.data[i];
            }

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector& operator+=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(size_t i = 0; i < std::min(Size, OtherSize); i++)
                this->data[i] += vec[i];

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr auto operator-(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            using OutType = Vector<std::common_type_t<I, OI>, std::max(Alignment, OtherAlignment), std::max(Size, OtherSize)>;

            size_t size = std::min(Size, OtherSize);

            OutType result(*this);
            for(size_t i = 0; i < size; i++)
                result[i] -= vec.data[i];

            if constexpr(OtherSize > Size)
            {
                for(size_t i = size; i < OtherSize; i++)
                    result[i] = -vec.data[i];
            }

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector& operator-=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(size_t i = 0; i < std::min(Size, OtherSize); i++)
                this->data[i] -= vec[i];

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr auto operator*(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            using OutType = Vector<std::common_type_t<I, OI>, std::max(Alignment, OtherAlignment), std::max(Size, OtherSize)>;

            size_t size = std::min(Size, OtherSize);

            OutType result(*this, 0);
            for(size_t i = 0; i < size; i++)
                result[i] *= vec.data[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector& operator*=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(size_t i = 0; i < std::min(Size, OtherSize); i++)
                this->data[i] *= vec[i];

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr auto operator/(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            using OutType = Vector<std::common_type_t<I, OI>, std::max(Alignment, OtherAlignment), std::max(Size, OtherSize)>;

            size_t size = std::min(Size, OtherSize);

            OutType result(*this, 0);
            for(size_t i = 0; i < size; i++)
                result[i] /= vec.data[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherSize>
        constexpr Vector& operator/=(const Vector<OI, OtherAlignment, OtherSize>& vec) const noexcept
        {
            for(size_t i = 0; i < std::min(Size, OtherSize); i++)
                this->data[i] /= vec[i];

            return *this;
        }

        bool operator==(const Vector& vec) const noexcept;

        constexpr auto GetLength() const noexcept
        {
            using OutType = std::common_type_t<I, float>;

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

        template<typename OI, size_t OtherAlignment>
        constexpr auto Dot(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            using OutType = std::common_type_t<I, OI, float>;

            OutType result = 0;
            for(size_t i = 0; i < Size; i++)
                result += this->data[i] * vec.data[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment>
        constexpr auto Cos(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            using OutType = std::common_type_t<I, OI, float>;

            OutType result = 0;
            for(size_t i = 0; i < Size; i++)
                result += this->data[i] * vec.data[i];

            result *= GetInvertedLength();
            result *= vec.GetInvertedLength();

            return result;
        }

        //use it when we know that both vectors are normalized
        template<typename OI, size_t OtherAlignment>
        constexpr auto RawCos(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            return Dot(vec);
        }

        template<typename OI, size_t OtherAlignment>
        constexpr auto Project(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            using OutType = std::common_type_t<I, OI, float>;

            OutType result = 0;
            for(size_t i = 0; i < Size; i++)
                result += this->data[i] * vec.data[i];

            result *= vec.GetInvertedLength();

            return Vector<OutType, std::max(Alignment, OtherAlignment), Size>(vec) * result;
        }

        template<typename OI, size_t OtherAlignment>
        requires(Size == 3)
        constexpr auto Cross(const Vector<OI, OtherAlignment, Size>& vec) const noexcept
        {
            //x = v0y * v1z - v1y * v0z;
            //y = -(v0x * v1z - v1x * v0z);
            //z = v0x * v1y - v1x * v0y

            using OutType = std::common_type_t<I, OI, float>;

            return Vector<OutType, std::max(Alignment, OtherAlignment), Size>(this->data[1] * vec[2] - vec[1] * this->data[2],
                                                                              -(this->data[0] * vec[2] - vec[0] * this->data[2]),
                                                                              this->data[0] * vec[1] - vec[0] * this->data[1]);
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

        /*
        TODO
        template<size_t Offset, size_t ViewSize>
        requires (Offset < Size && Offset + ViewSize <= Size)
        VectorView<I, ViewSize> TakeView() noexcept;

        template<size_t Offset, size_t ViewSize>
        requires (Offset < Size && Offset + ViewSize <= Size)
        VectorView<I, ViewSize> TakeView() const noexcept;
        */
    private:
        alignas(Alignment) I data[Size];
    };

    template<typename I>
    Vector(I fill_value) -> Vector<I, alignof(I), 1>;

    template<typename... Args>
    Vector(Args...) -> Vector<std::common_type_t<Args...>, alignof(std::common_type_t<Args...>), sizeof...(Args)>;

    template<typename I, size_t Alignment, size_t Size>
    Vector(const Vector<I, Alignment, Size>&) -> Vector<I, Alignment, Size>;

    //std compat
    template<typename I, size_t Alignment, size_t Size>
    constexpr auto begin(const Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetIterator();
    }

    template<typename I, size_t Alignment, size_t Size>
    constexpr auto begin(Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetIterator();
    }

    template<typename I, size_t Alignment, size_t Size>
    constexpr auto begin(Vector<I, Alignment, Size>&& vec) noexcept
    {
        return std::move(vec).GetIterator();
    }

    template<typename I, size_t Alignment, size_t Size>
    constexpr auto end(const Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Size>
    constexpr auto end(Vector<I, Alignment, Size>& vec) noexcept
    {
        return vec.GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Size>
    constexpr auto end(Vector<I, Alignment, Size>&& vec) noexcept
    {
        return std::move(vec).GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Size>
    constexpr auto size(const Vector<I, Alignment, Size>& vec) noexcept
    {
        return Size;
    }

    namespace GLSL
    {
        namespace Scalar
        {
            using Vector1i32 = Vector<int32_t, alignof(int32_t), 1>;
            using Vector2i32 = Vector<int32_t, alignof(int32_t), 2>;
            using Vector3i32 = Vector<int32_t, alignof(int32_t), 3>;
            using Vector4i32 = Vector<int32_t, alignof(int32_t), 4>;

            using Vector1ui32 = Vector<uint32_t, alignof(uint32_t), 1>;
            using Vector2ui32 = Vector<uint32_t, alignof(uint32_t), 2>;
            using Vector3ui32 = Vector<uint32_t, alignof(uint32_t), 3>;
            using Vector4ui32 = Vector<uint32_t, alignof(uint32_t), 4>;

            using Vector1f32 = Vector<float, alignof(float), 1>;
            using Vector2f32 = Vector<float, alignof(float), 2>;
            using Vector3f32 = Vector<float, alignof(float), 3>;
            using Vector4f32 = Vector<float, alignof(float), 4>;

            using Vector1f64 = Vector<double, alignof(double), 1>;
            using Vector2f64 = Vector<double, alignof(double), 2>;
            using Vector3f64 = Vector<double, alignof(double), 3>;
            using Vector4f64 = Vector<double, alignof(double), 4>;

            using Vector1b32 = Vector1i32;
            using Vector2b32 = Vector2i32;
            using Vector3b32 = Vector3i32;
            using Vector4b32 = Vector4i32;
        };

        namespace STD140
        {
            using Vector1i32 = Vector<int32_t, alignof(int32_t), 1>;
            using Vector2i32 = Vector<int32_t, alignof(int32_t) * 2, 2>;
            using Vector3i32 = Vector<int32_t, alignof(int32_t) * 4, 3>;
            using Vector4i32 = Vector<int32_t, alignof(int32_t) * 4, 4>;

            using Vector1ui32 = Vector<uint32_t, alignof(uint32_t), 1>;
            using Vector2ui32 = Vector<uint32_t, alignof(uint32_t) * 2, 2>;
            using Vector3ui32 = Vector<uint32_t, alignof(uint32_t) * 4, 3>;
            using Vector4ui32 = Vector<uint32_t, alignof(uint32_t) * 4, 4>;

            using Vector1f32 = Vector<float, alignof(float), 1>;
            using Vector2f32 = Vector<float, alignof(float) * 2, 2>;
            using Vector3f32 = Vector<float, alignof(float) * 4, 3>;
            using Vector4f32 = Vector<float, alignof(float) * 4, 4>;

            using Vector1f64 = Vector<double, alignof(double), 1>;
            using Vector2f64 = Vector<double, alignof(double) * 2, 2>;
            using Vector3f64 = Vector<double, alignof(double) * 4, 3>;
            using Vector4f64 = Vector<double, alignof(double) * 4, 4>;

            using Vector1b32 = Vector1i32;
            using Vector2b32 = Vector2i32;
            using Vector3b32 = Vector3i32;
            using Vector4b32 = Vector4i32;
        };

        namespace STD430
        {
            using Vector1i32 = Vector<int32_t, alignof(int32_t), 1>;
            using Vector2i32 = Vector<int32_t, alignof(int32_t) * 2, 2>;
            using Vector3i32 = Vector<int32_t, alignof(int32_t) * 4, 3>;
            using Vector4i32 = Vector<int32_t, alignof(int32_t) * 4, 4>;

            using Vector1ui32 = Vector<uint32_t, alignof(uint32_t), 1>;
            using Vector2ui32 = Vector<uint32_t, alignof(uint32_t) * 2, 2>;
            using Vector3ui32 = Vector<uint32_t, alignof(uint32_t) * 4, 3>;
            using Vector4ui32 = Vector<uint32_t, alignof(uint32_t) * 4, 4>;

            using Vector1f32 = Vector<float, alignof(float), 1>;
            using Vector2f32 = Vector<float, alignof(float) * 2, 2>;
            using Vector3f32 = Vector<float, alignof(float) * 4, 3>;
            using Vector4f32 = Vector<float, alignof(float) * 4, 4>;

            using Vector1f64 = Vector<double, alignof(double), 1>;
            using Vector2f64 = Vector<double, alignof(double) * 2, 2>;
            using Vector3f64 = Vector<double, alignof(double) * 4, 3>;
            using Vector4f64 = Vector<double, alignof(double) * 4, 4>;

            using Vector1b32 = Vector1i32;
            using Vector2b32 = Vector2i32;
            using Vector3b32 = Vector3i32;
            using Vector4b32 = Vector4i32;
        };
    };
};