#pragma once

#include "Vector.hpp"
#include <cassert>

namespace Core
{
    template<typename I, size_t Alignment, size_t Width, size_t Height>
    requires std::same_as<std::remove_cvref_t<I>, I> && std::is_arithmetic_v<I> && (Width > 0) && (Height > 0)
    class Matrix
    {
    public:
        using Row = Vector<I, Alignment, Width>;

        using Iterator = Row*;
        using ConstIterator = const Row*;

        Matrix() = default;
        ~Matrix() = default;
        Matrix(const Matrix&) = default;
        Matrix(Matrix&&) = default;
        Matrix& operator=(const Matrix&) = default;
        Matrix& operator=(Matrix&&) = default;

        constexpr Matrix(I fill_value) noexcept
        {
            for(size_t i = 0; i < Height; i++)
                this->data[i] = Row(fill_value);
        }

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        constexpr Matrix(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Height, OtherHeight); i++)
                this->data[i] = mat[i];
        }

        template<typename OI, size_t OtherAlignment, size_t OtherHeight, size_t OtherWidth>
        constexpr Matrix(const Matrix<OI, OtherAlignment, OtherHeight, OtherWidth>& mat, I fill_value) noexcept
        {
            for(size_t i = 0; i < Width; i++)
            {
                if(i < OtherWidth)
                    this->data[i] = Row(mat[i], fill_value);
                else
                    this->data[i] = Row(fill_value);
            }
        }

        template<typename... Args>
        requires(std::constructible_from<Row, Args> && ...) && (sizeof...(Args) <= Height)
        constexpr Matrix(Args&&... args) noexcept((std::is_nothrow_constructible_v<Row, Args> && ...))
            : data(std::forward<Args>(args)...)
        {}

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        constexpr Matrix& operator=(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Height, OtherHeight); i++)
                this->data[i] = mat[i];
        }

        constexpr static Matrix Identity(I identity_value) noexcept
        requires(Width == Height)
        {
            Matrix out;

            for(size_t i = 0; i < Height; i++)
            {
                for(size_t j = 0; j < Width; j++)
                {
                    I value = (i == j ? identity_value : 0);
                    out[i][j] = value;
                }
            }

            return out;
        }

        constexpr Row* GetData() noexcept
        {
            return this->data;
        }

        constexpr const Row* GetData() const noexcept
        {
            return this->data;
        }

        constexpr Row& operator[](size_t index) noexcept
        {
            assert(index < Height);

            return this->data[index];
        }

        constexpr const Row& operator[](size_t index) const noexcept
        {
            assert(index < Height);

            return this->data[index];
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix operator+(T value) const noexcept
        {
            Matrix out(*this);

            for(auto& row: out)
                for(auto& comp: row)
                    comp += value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix& operator+=(T value) noexcept
        {
            for(auto& row: *this)
                for(auto& comp: row)
                    comp += value;

            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix operator-(T value) const noexcept
        {
            Matrix out(*this);

            for(auto& row: out)
                for(auto& comp: row)
                    comp -= value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix& operator-=(T value) noexcept
        {
            for(auto& row: *this)
                for(auto& comp: row)
                    comp -= value;

            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix operator*(T value) const noexcept
        {
            Matrix out(*this);

            for(auto& row: out)
                for(auto& comp: row)
                    comp *= value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix& operator*=(T value) noexcept
        {
            for(auto& row: *this)
                for(auto& comp: row)
                    comp *= value;

            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix operator/(T value) const noexcept
        {
            Matrix out(*this);

            for(auto& row: out)
                for(auto& comp: row)
                    comp /= value;

            return out;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        constexpr Matrix& operator/=(T value) noexcept
        {
            for(auto& row: *this)
                for(auto& comp: row)
                    comp /= value;

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        constexpr Matrix operator+(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            Matrix result(*this);
            for(size_t i = 0; i < std::min(Height, OtherHeight); i++)
                result[i] += mat[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        constexpr Matrix& operator+=(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Height, OtherHeight); i++)
                this->data[i] += mat[i];

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        constexpr Matrix operator-(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            Matrix result(*this);
            for(size_t i = 0; i < std::min(Height, OtherHeight); i++)
                result[i] -= mat[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        constexpr Matrix& operator-=(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Height, OtherHeight); i++)
                this->data[i] -= mat[i];

            return *this;
        }

        template<size_t NewAlignment = Alignment>
        constexpr auto Transposed() const noexcept
        {
            Matrix<I, NewAlignment, Height, Width> out;
            for(size_t i = 0; i < Height; i++)
            {
                for(size_t j = 0; j < Width; j++)
                {
                    out[j][i] = this->data[i][j];
                }
            }

            return out;
        }

        constexpr Matrix& Transpose() noexcept
        requires(Width == Height)
        {
            for(size_t i = 0; i < Height; i++)
            {
                for(size_t j = 0; j < i; j++)
                {
                    std::swap(this->data[i][j], this->data[j][i]);
                }
            }

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
        requires(Width == OtherHeight)
        constexpr auto operator*(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>& mat) noexcept
        {
            Matrix<I, Alignment, OtherWidth, Height> out;

            auto mat_transposed = mat.Transposed();
            for(size_t i = 0; i < Height; i++)
            {
                for(size_t j = 0; j < OtherWidth; j++)
                {
                    out[i][j] = this->data[i].Dot(mat_transposed[j]);
                }
            }

            return out;
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
            return Iterator(this->data + Width);
        }

        constexpr ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(this->data + Width);
        }

        consteval static size_t GetWidth() noexcept
        {
            return Width;
        }

        consteval static size_t GetHeight() noexcept
        {
            return Height;
        }
    private:
        Row data[Height];
    };

    template<typename VI, size_t VectorAlignment, size_t VectorSize, typename MI, size_t MatrixAlignment, size_t MatrixWidth, size_t MatrixHeight>
    requires(VectorSize == MatrixHeight)
    constexpr auto operator*(const Vector<VI, VectorAlignment, VectorSize>& vec, const Matrix<MI, MatrixAlignment, MatrixWidth, MatrixHeight>& mat) noexcept
    {
        Vector<VI, VectorAlignment, MatrixWidth> out;

        auto mat_transposed = mat.Transposed();
        for(size_t i = 0; i < MatrixWidth; i++)
        {
            out[i] = vec.Dot(mat_transposed[i]);
        }

        return out;
    }

    template<typename I>
    Matrix(I fill_value) -> Matrix<I, alignof(I), 1, 1>;

    template<typename OI, size_t OtherAlignment, size_t OtherWidth, size_t OtherHeight>
    Matrix(const Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>&) -> Matrix<OI, OtherAlignment, OtherWidth, OtherHeight>;

    template<typename VI, size_t VectorAlignment, size_t VectorSize, typename... Args>
    Matrix(const Vector<VI, VectorAlignment, VectorSize>&, Args&&... args) -> Matrix<VI, VectorAlignment, VectorSize, sizeof...(Args) + 1>;

    //std compat
    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto begin(const Matrix<I, Alignment, Width, Height>& mat) noexcept
    {
        return mat.GetIterator();
    }

    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto begin(Matrix<I, Alignment, Width, Height>& mat) noexcept
    {
        return mat.GetIterator();
    }

    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto begin(Matrix<I, Alignment, Width, Height>&& mat) noexcept
    {
        return std::move(mat).GetIterator();
    }

    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto end(const Matrix<I, Alignment, Width, Height>& mat) noexcept
    {
        return mat.GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto end(Matrix<I, Alignment, Width, Height>& mat) noexcept
    {
        return mat.GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto end(Matrix<I, Alignment, Width, Height>&& mat) noexcept
    {
        return std::move(mat).GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Width, size_t Height>
    constexpr auto size(const Matrix<I, Alignment, Width, Height>& mat) noexcept
    {
        return Height;
    }

    namespace GLSL
    {
        namespace Scalar
        {
            using Matrix2x2i32 = Matrix<int32_t, alignof(int32_t), 2, 2>;
            using Matrix2x3i32 = Matrix<int32_t, alignof(int32_t), 2, 3>;
            using Matrix2x4i32 = Matrix<int32_t, alignof(int32_t), 2, 4>;
            using Matrix3x2i32 = Matrix<int32_t, alignof(int32_t), 3, 2>;
            using Matrix3x3i32 = Matrix<int32_t, alignof(int32_t), 3, 3>;
            using Matrix3x4i32 = Matrix<int32_t, alignof(int32_t), 3, 4>;
            using Matrix4x2i32 = Matrix<int32_t, alignof(int32_t), 4, 2>;
            using Matrix4x3i32 = Matrix<int32_t, alignof(int32_t), 4, 3>;
            using Matrix4x4i32 = Matrix<int32_t, alignof(int32_t), 4, 4>;

            using Matrix2x2ui32 = Matrix<uint32_t, alignof(uint32_t), 2, 2>;
            using Matrix2x3ui32 = Matrix<uint32_t, alignof(uint32_t), 2, 3>;
            using Matrix2x4ui32 = Matrix<uint32_t, alignof(uint32_t), 2, 4>;
            using Matrix3x2ui32 = Matrix<uint32_t, alignof(uint32_t), 3, 2>;
            using Matrix3x3ui32 = Matrix<uint32_t, alignof(uint32_t), 3, 3>;
            using Matrix3x4ui32 = Matrix<uint32_t, alignof(uint32_t), 3, 4>;
            using Matrix4x2ui32 = Matrix<uint32_t, alignof(uint32_t), 4, 2>;
            using Matrix4x3ui32 = Matrix<uint32_t, alignof(uint32_t), 4, 3>;
            using Matrix4x4ui32 = Matrix<uint32_t, alignof(uint32_t), 4, 4>;

            using Matrix2x2f32 = Matrix<float, alignof(float), 2, 2>;
            using Matrix2x3f32 = Matrix<float, alignof(float), 2, 3>;
            using Matrix2x4f32 = Matrix<float, alignof(float), 2, 4>;
            using Matrix3x2f32 = Matrix<float, alignof(float), 3, 2>;
            using Matrix3x3f32 = Matrix<float, alignof(float), 3, 3>;
            using Matrix3x4f32 = Matrix<float, alignof(float), 3, 4>;
            using Matrix4x2f32 = Matrix<float, alignof(float), 4, 2>;
            using Matrix4x3f32 = Matrix<float, alignof(float), 4, 3>;
            using Matrix4x4f32 = Matrix<float, alignof(float), 4, 4>;

            using Matrix2x2f64 = Matrix<double, alignof(double), 2, 2>;
            using Matrix2x3f64 = Matrix<double, alignof(double), 2, 3>;
            using Matrix2x4f64 = Matrix<double, alignof(double), 2, 4>;
            using Matrix3x2f64 = Matrix<double, alignof(double), 3, 2>;
            using Matrix3x3f64 = Matrix<double, alignof(double), 3, 3>;
            using Matrix3x4f64 = Matrix<double, alignof(double), 3, 4>;
            using Matrix4x2f64 = Matrix<double, alignof(double), 4, 2>;
            using Matrix4x3f64 = Matrix<double, alignof(double), 4, 3>;
            using Matrix4x4f64 = Matrix<double, alignof(double), 4, 4>;

            using Matrix2x2b32 = Matrix2x2i32;
            using Matrix2x3b32 = Matrix2x3i32;
            using Matrix2x4b32 = Matrix2x4i32;
            using Matrix3x2b32 = Matrix3x2i32;
            using Matrix3x3b32 = Matrix3x3i32;
            using Matrix3x4b32 = Matrix3x4i32;
            using Matrix4x2b32 = Matrix4x2i32;
            using Matrix4x3b32 = Matrix4x3i32;
            using Matrix4x4b32 = Matrix4x4i32;
        };

        namespace STD140
        {
            using Matrix2x2i32 = Matrix<int32_t, alignof(int32_t) * 4, 2, 2>;
            using Matrix2x3i32 = Matrix<int32_t, alignof(int32_t) * 4, 2, 3>;
            using Matrix2x4i32 = Matrix<int32_t, alignof(int32_t) * 4, 2, 4>;
            using Matrix3x2i32 = Matrix<int32_t, alignof(int32_t) * 4, 3, 2>;
            using Matrix3x3i32 = Matrix<int32_t, alignof(int32_t) * 4, 3, 3>;
            using Matrix3x4i32 = Matrix<int32_t, alignof(int32_t) * 4, 3, 4>;
            using Matrix4x2i32 = Matrix<int32_t, alignof(int32_t) * 4, 4, 2>;
            using Matrix4x3i32 = Matrix<int32_t, alignof(int32_t) * 4, 4, 3>;
            using Matrix4x4i32 = Matrix<int32_t, alignof(int32_t) * 4, 4, 4>;

            using Matrix2x2ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 2, 2>;
            using Matrix2x3ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 2, 3>;
            using Matrix2x4ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 2, 4>;
            using Matrix3x2ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 3, 2>;
            using Matrix3x3ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 3, 3>;
            using Matrix3x4ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 3, 4>;
            using Matrix4x2ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 4, 2>;
            using Matrix4x3ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 4, 3>;
            using Matrix4x4ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 4, 4>;

            using Matrix2x2f32 = Matrix<float, alignof(float) * 4, 2, 2>;
            using Matrix2x3f32 = Matrix<float, alignof(float) * 4, 2, 3>;
            using Matrix2x4f32 = Matrix<float, alignof(float) * 4, 2, 4>;
            using Matrix3x2f32 = Matrix<float, alignof(float) * 4, 3, 2>;
            using Matrix3x3f32 = Matrix<float, alignof(float) * 4, 3, 3>;
            using Matrix3x4f32 = Matrix<float, alignof(float) * 4, 3, 4>;
            using Matrix4x2f32 = Matrix<float, alignof(float) * 4, 4, 2>;
            using Matrix4x3f32 = Matrix<float, alignof(float) * 4, 4, 3>;
            using Matrix4x4f32 = Matrix<float, alignof(float) * 4, 4, 4>;

            using Matrix2x2f64 = Matrix<double, alignof(double) * 2, 2, 2>;
            using Matrix2x3f64 = Matrix<double, alignof(double) * 2, 2, 3>;
            using Matrix2x4f64 = Matrix<double, alignof(double) * 2, 2, 4>;
            using Matrix3x2f64 = Matrix<double, alignof(double) * 4, 3, 2>;
            using Matrix3x3f64 = Matrix<double, alignof(double) * 4, 3, 3>;
            using Matrix3x4f64 = Matrix<double, alignof(double) * 4, 3, 4>;
            using Matrix4x2f64 = Matrix<double, alignof(double) * 4, 4, 2>;
            using Matrix4x3f64 = Matrix<double, alignof(double) * 4, 4, 3>;
            using Matrix4x4f64 = Matrix<double, alignof(double) * 4, 4, 4>;

            using Matrix2x2b32 = Matrix2x2i32;
            using Matrix2x3b32 = Matrix2x3i32;
            using Matrix2x4b32 = Matrix2x4i32;
            using Matrix3x2b32 = Matrix3x2i32;
            using Matrix3x3b32 = Matrix3x3i32;
            using Matrix3x4b32 = Matrix3x4i32;
            using Matrix4x2b32 = Matrix4x2i32;
            using Matrix4x3b32 = Matrix4x3i32;
            using Matrix4x4b32 = Matrix4x4i32;
        };

        namespace STD430
        {
            using Matrix2x2i32 = Matrix<int32_t, alignof(int32_t) * 2, 2, 2>;
            using Matrix2x3i32 = Matrix<int32_t, alignof(int32_t) * 2, 2, 3>;
            using Matrix2x4i32 = Matrix<int32_t, alignof(int32_t) * 2, 2, 4>;
            using Matrix3x2i32 = Matrix<int32_t, alignof(int32_t) * 4, 3, 2>;
            using Matrix3x3i32 = Matrix<int32_t, alignof(int32_t) * 4, 3, 3>;
            using Matrix3x4i32 = Matrix<int32_t, alignof(int32_t) * 4, 3, 4>;
            using Matrix4x2i32 = Matrix<int32_t, alignof(int32_t) * 4, 4, 2>;
            using Matrix4x3i32 = Matrix<int32_t, alignof(int32_t) * 4, 4, 3>;
            using Matrix4x4i32 = Matrix<int32_t, alignof(int32_t) * 4, 4, 4>;

            using Matrix2x2ui32 = Matrix<uint32_t, alignof(uint32_t) * 2, 2, 2>;
            using Matrix2x3ui32 = Matrix<uint32_t, alignof(uint32_t) * 2, 2, 3>;
            using Matrix2x4ui32 = Matrix<uint32_t, alignof(uint32_t) * 2, 2, 4>;
            using Matrix3x2ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 3, 2>;
            using Matrix3x3ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 3, 3>;
            using Matrix3x4ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 3, 4>;
            using Matrix4x2ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 4, 2>;
            using Matrix4x3ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 4, 3>;
            using Matrix4x4ui32 = Matrix<uint32_t, alignof(uint32_t) * 4, 4, 4>;

            using Matrix2x2f32 = Matrix<float, alignof(float) * 2, 2, 2>;
            using Matrix2x3f32 = Matrix<float, alignof(float) * 2, 2, 3>;
            using Matrix2x4f32 = Matrix<float, alignof(float) * 2, 2, 4>;
            using Matrix3x2f32 = Matrix<float, alignof(float) * 4, 3, 2>;
            using Matrix3x3f32 = Matrix<float, alignof(float) * 4, 3, 3>;
            using Matrix3x4f32 = Matrix<float, alignof(float) * 4, 3, 4>;
            using Matrix4x2f32 = Matrix<float, alignof(float) * 4, 4, 2>;
            using Matrix4x3f32 = Matrix<float, alignof(float) * 4, 4, 3>;
            using Matrix4x4f32 = Matrix<float, alignof(float) * 4, 4, 4>;

            using Matrix2x2f64 = Matrix<double, alignof(double) * 2, 2, 2>;
            using Matrix2x3f64 = Matrix<double, alignof(double) * 2, 2, 3>;
            using Matrix2x4f64 = Matrix<double, alignof(double) * 2, 2, 4>;
            using Matrix3x2f64 = Matrix<double, alignof(double) * 4, 3, 2>;
            using Matrix3x3f64 = Matrix<double, alignof(double) * 4, 3, 3>;
            using Matrix3x4f64 = Matrix<double, alignof(double) * 4, 3, 4>;
            using Matrix4x2f64 = Matrix<double, alignof(double) * 4, 4, 2>;
            using Matrix4x3f64 = Matrix<double, alignof(double) * 4, 4, 3>;
            using Matrix4x4f64 = Matrix<double, alignof(double) * 4, 4, 4>;

            using Matrix2x2b32 = Matrix2x2i32;
            using Matrix2x3b32 = Matrix2x3i32;
            using Matrix2x4b32 = Matrix2x4i32;
            using Matrix3x2b32 = Matrix3x2i32;
            using Matrix3x3b32 = Matrix3x3i32;
            using Matrix3x4b32 = Matrix3x4i32;
            using Matrix4x2b32 = Matrix4x2i32;
            using Matrix4x3b32 = Matrix4x3i32;
            using Matrix4x4b32 = Matrix4x4i32;
        };
    };
};