#pragma once

#include "Vector.hpp"
#include <cassert>

namespace Core
{
    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    requires std::same_as<std::remove_cvref_t<I>, I> && std::is_arithmetic_v<I> && (Rows > 0) && (Columns > 0)
    class Matrix
    {
    public:
        using Row = Vector<I, Alignment, Columns>;

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
            for(size_t i = 0; i < Rows; i++)
                this->data[i] = Row(fill_value);
        }

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Rows, OtherRows); i++)
                this->data[i] = mat[i];
        }

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat, I fill_value) noexcept
        {
            for(size_t i = 0; i < Rows; i++)
            {
                if(i < OtherRows)
                    this->data[i] = Row(mat[i], fill_value);
                else
                    this->data[i] = Row(fill_value);
            }
        }

        template<typename... Args>
        requires(std::constructible_from<Row, Args> && ...) && (sizeof...(Args) <= Rows)
        constexpr Matrix(Args&&... args) noexcept((std::is_nothrow_constructible_v<Row, Args> && ...))
            : data(std::forward<Args>(args)...)
        {}

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix& operator=(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Rows, OtherRows); i++)
                this->data[i] = mat[i];
        }

        constexpr static Matrix Identity(I identity_value) noexcept
        requires(Rows == Columns)
        {
            Matrix out;

            for(size_t i = 0; i < Rows; i++)
            {
                for(size_t j = 0; j < Columns; j++)
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
            assert(index < Rows);

            return this->data[index];
        }

        constexpr const Row& operator[](size_t index) const noexcept
        {
            assert(index < Rows);

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

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix operator+(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
            Matrix result(*this);
            for(size_t i = 0; i < std::min(Rows, OtherRows); i++)
                result[i] += mat[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix& operator+=(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Rows, OtherRows); i++)
                this->data[i] += mat[i];

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix operator-(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
            Matrix result(*this);
            for(size_t i = 0; i < std::min(Rows, OtherRows); i++)
                result[i] -= mat[i];

            return result;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        constexpr Matrix& operator-=(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
            for(size_t i = 0; i < std::min(Rows, OtherRows); i++)
                this->data[i] -= mat[i];

            return *this;
        }

        template<size_t NewAlignment = Alignment>
        constexpr Matrix<I, NewAlignment, Rows, Columns> Transposed() const noexcept
        {
            Matrix<I, Alignment, Rows, Columns> out;
            for(size_t i = 0; i < Rows; i++)
            {
                for(size_t j = 0; j < Columns; j++)
                {
                    out[j][i] = this->data[i][j];
                }
            }

            return out;
        }

        constexpr Matrix& Transpose() noexcept
        requires(Rows == Columns)
        {
            for(size_t i = 0; i < Rows; i++)
            {
                for(size_t j = 0; j < i; j++)
                {
                    std::swap(this->data[i][j], this->data[j][i]);
                }
            }

            return *this;
        }

        template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
        requires(Columns == OtherRows)
        constexpr auto operator*(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>& mat) noexcept
        {
#error TODO:
            Matrix<I, Alignment, OtherColumns, Rows> out;
        }

#pragma message("TODO")
        /*
        mul matrix

        mul vector

        mul matrix in vec class
        */

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
            return Iterator(this->data + Rows);
        }

        constexpr ConstIterator GetSentinel() const noexcept
        {
            return ConstIterator(this->data + Rows);
        }

        consteval static size_t GetRowCount() noexcept
        {
            return Rows;
        }

        consteval static size_t GetColumnCount() noexcept
        {
            return Columns;
        }
    private:
        Row data[Rows];
    };

    template<typename I>
    Matrix(I fill_value) -> Matrix<I, alignof(I), 1, 1>;

    template<typename OI, size_t OtherAlignment, size_t OtherColumns, size_t OtherRows>
    Matrix(const Matrix<OI, OtherAlignment, OtherColumns, OtherRows>&) -> Matrix<OI, OtherAlignment, OtherColumns, OtherRows>;

#pragma message("Deduction guide for variadic Vectors")

    //std compat
    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto begin(const Matrix<I, Alignment, Columns, Rows>& mat) noexcept
    {
        return mat.GetIterator();
    }

    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto begin(Matrix<I, Alignment, Columns, Rows>& mat) noexcept
    {
        return mat.GetIterator();
    }

    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto begin(Matrix<I, Alignment, Columns, Rows>&& mat) noexcept
    {
        return std::move(mat).GetIterator();
    }

    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto end(const Matrix<I, Alignment, Columns, Rows>& mat) noexcept
    {
        return mat.GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto end(Matrix<I, Alignment, Columns, Rows>& mat) noexcept
    {
        return mat.GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto end(Matrix<I, Alignment, Columns, Rows>&& mat) noexcept
    {
        return std::move(mat).GetSentinel();
    }

    template<typename I, size_t Alignment, size_t Columns, size_t Rows>
    constexpr auto size(const Matrix<I, Alignment, Columns, Rows>& mat) noexcept
    {
        return Rows;
    }

#pragma message("GLSL types")

};