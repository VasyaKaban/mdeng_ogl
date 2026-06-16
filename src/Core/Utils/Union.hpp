#pragma once

#include <cassert>
#include "Traits.hpp"
#include "Binary.hpp"
#include "Variadic.hpp"

namespace Core
{
    namespace Detail
    {
        struct UnionMetrics
        {
            size_t alignment;
            size_t size;
        };

        template<typename T, typename... Types>
        constexpr void AdjustUnionMetrics(UnionMetrics& metrics) noexcept
        {
            if(alignof(T) > metrics.alignment)
                metrics.alignment = alignof(T);

            if(sizeof(T) > metrics.size)
                metrics.size = sizeof(T);

            if constexpr(sizeof...(Types) != 0)
                AdjustUnionMetrics<Types...>(metrics);
        }

        template<typename T, typename... Types>
        constexpr UnionMetrics GetUnionMetrics() noexcept
        {
            UnionMetrics metrics = {.alignment = 0, .size = 0};
            AdjustUnionMetrics<T, Types...>(metrics);

            Align(metrics.size, metrics.alignment);

            return metrics;
        }

        template<typename T>
        constexpr void UnionDestructorWrapper(const char* data) noexcept
        {
            reinterpret_cast<const T*>(data)->~T();
        }

        template<typename T>
        constexpr void UnionCopyConstructorWrapper(char* to, const char* from)
        {
            new(to) T(*reinterpret_cast<const T*>(from));
        }

        template<typename T>
        constexpr void UnionMoveConstructorWrapper(char* to, char* from)
        {
            new(to) T(std::move(*reinterpret_cast<T*>(from)));
        }

        template<typename F, bool IsConst, bool IsRVlaue, typename T>
        constexpr void UnionVisitorWrapper(F&& visitor, const char* data)
        {
            if constexpr(IsConst)
            {
                if constexpr(IsRVlaue)
                    std::forward<F>(visitor)(std::move(*reinterpret_cast<const T*>(data)));
                else
                    std::forward<F>(visitor)(*reinterpret_cast<const T*>(data));
            }
            else
            {
                if constexpr(IsRVlaue)
                    std::forward<F>(visitor)(std::move(*reinterpret_cast<T*>(const_cast<char*>(data))));
                else
                    std::forward<F>(visitor)(*reinterpret_cast<T*>(const_cast<char*>(data)));
            }
        }
    };

    constexpr inline ptrdiff_t NON_ACTIVE_UNION_INDEX = -1;

    template<typename... Types>
    requires(sizeof...(Types) > 0 && (sizeof...(Types) - 1 <= std::numeric_limits<int64_t>::max())) && (std::same_as<std::remove_cvref_t<Types>, Types> && ...)
    class Union
    {
        constexpr static Detail::UnionMetrics METRICS = Detail::GetUnionMetrics<Types...>();
        using VariadicTypes = Variadic<Types...>;
    public:
        template<size_t Index>
        requires(Index < sizeof...(Types))
        using TypeOfIndex = typename VariadicTypes::template TypeOfIndex<Index>;

        template<typename T>
        requires(std::same_as<T, Types> || ...)
        constexpr static size_t IndexOfType = VariadicTypes::template IndexOfType<T>;

        Union() noexcept
            : active_index(NON_ACTIVE_UNION_INDEX)
        {}

        template<std::integral auto Index, typename... Args>
        requires(Index >= 0 && Index < sizeof...(Types)) && std::constructible_from<TypeOfIndex<Index>, Args...>
        Union(NonType<Index>, Args&&... args) noexcept(std::is_nothrow_constructible_v<TypeOfIndex<Index>, Args...>)
        {
            new(this->data) TypeOfIndex<Index>(std::forward<Args>(args)...);

            this->active_index = Index;
        }

        template<typename T, typename... Args>
        requires(std::same_as<T, Types> || ...)
        Union(InPlaceType<T>, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        {
            new(this->data) T(std::forward<Args>(args)...);

            this->active_index = IndexOfType<T>;
        }

        ~Union()
        {
            constexpr static void (*DESTRUCTORS[])(const char* data) = {&Detail::UnionDestructorWrapper<Types>...};

            if(this->active_index != NON_ACTIVE_UNION_INDEX)
                DESTRUCTORS[this->active_index](this->data);
        }

        Union(const Union& un) noexcept((std::is_nothrow_copy_constructible_v<Types> && ...))
        requires(std::copy_constructible<Types> && ...)
        {
            constexpr static void (*COPY_CONSTRUCTORS[])(char* to, const char* from) = {&Detail::UnionCopyConstructorWrapper<Types>...};

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                COPY_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;
        }

        Union(Union&& un) noexcept((std::is_nothrow_move_constructible_v<Types> && ...))
        requires(std::move_constructible<Types> && ...)
        {
            constexpr static void (*MOVE_CONSTRUCTORS[])(char* to, char* from) = {&Detail::UnionMoveConstructorWrapper<Types>...};

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                MOVE_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;
        }

        Union& operator=(const Union& un) noexcept((std::is_nothrow_copy_constructible_v<Types> && ...))
        requires(std::copy_constructible<Types> && ...)
        {
            constexpr static void (*COPY_CONSTRUCTORS[])(char* to, const char* from) = {&Detail::UnionCopyConstructorWrapper<Types>...};

            this->Reset();

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                COPY_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;

            return *this;
        }

        Union& operator=(Union&& un) noexcept((std::is_nothrow_move_constructible_v<Types> && ...))
        requires(std::move_constructible<Types> && ...)
        {
            constexpr static void (*MOVE_CONSTRUCTORS[])(char* to, char* from) = {&Detail::UnionMoveConstructorWrapper<Types>...};

            this->Reset();

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                MOVE_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;

            return *this;
        }

        ptrdiff_t GetActiveIndex() const noexcept
        {
            return this->active_index;
        }

        void Reset() noexcept
        {
            if(this->active_index != NON_ACTIVE_UNION_INDEX)
            {
                this->~Union();

                this->active_index = NON_ACTIVE_UNION_INDEX;
            }
        }

        template<typename T>
        requires(std::same_as<T, Types> || ...)
        bool IsActive() const noexcept
        {
            return this->active_index == IndexOfType<T>;
        }

        //Get by Index
        template<size_t Index>
        requires(Index < sizeof...(Types))
        TypeOfIndex<Index>& Get() & noexcept
        {
            assert(this->active_index == Index);

            return *reinterpret_cast<TypeOfIndex<Index>*>(this->data);
        }

        template<size_t Index>
        requires(Index < sizeof...(Types))
        const TypeOfIndex<Index>& Get() const& noexcept
        {
            assert(this->active_index == Index);

            return *reinterpret_cast<const TypeOfIndex<Index>*>(this->data);
        }

        template<size_t Index>
        requires(Index < sizeof...(Types))
        TypeOfIndex<Index>&& Get() && noexcept
        {
            assert(this->active_index == Index);

            return std::move(*reinterpret_cast<TypeOfIndex<Index>*>(this->data));
        }

        template<size_t Index>
        requires(Index < sizeof...(Types))
        const TypeOfIndex<Index>&& Get() const&& noexcept
        {
            assert(this->active_index == Index);

            return std::move(*reinterpret_cast<const TypeOfIndex<Index>*>(this->data));
        }

        //Get by Type
        template<typename T>
        requires(std::same_as<T, Types> || ...)
        T& Get() noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return *reinterpret_cast<T*>(this->data);
        }

        template<typename T>
        requires(std::same_as<T, Types> || ...)
        const T& Get() const& noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return *reinterpret_cast<const T*>(this->data);
        }

        template<typename T>
        requires(std::same_as<T, Types> || ...)
        T&& Get() && noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return std::move(*reinterpret_cast<T*>(this->data));
        }

        template<typename T>
        requires(std::same_as<T, Types> || ...)
        const T&& Get() const&& noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return std::move(*reinterpret_cast<const T*>(this->data));
        }

        //Set by Index
        template<size_t Index, typename... Args>
        requires(Index < sizeof...(Types)) && std::constructible_from<TypeOfIndex<Index>, Args...>
        void Set(Args&&... args) noexcept(std::is_nothrow_constructible_v<TypeOfIndex<Index>, Args...>)
        {
            this->Reset();

            new(this->data) TypeOfIndex<Index>(std::forward<Args>(args)...);

            this->active_index = Index;
        }

        //Set by Type
        template<typename T, typename... Args>
        requires(std::same_as<T, Types> || ...) && std::constructible_from<T, Args...>
        void Set(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        {
            this->Reset();

            new(this->data) T(std::forward<Args>(args)...);

            this->active_index = IndexOfType<T>;
        }

        template<typename F>
        requires(std::invocable<F, Types&> && ...)
        bool Visit(F&& visitor) & noexcept((std::is_nothrow_invocable_v<F, Types&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, false, false, Types>...};

            VISITORS[this->active_index](std::forward<F>(visitor), this->data);

            return true;
        }

        template<typename F>
        requires(std::invocable<F, Types &&> && ...)
        bool Visit(F&& visitor) && noexcept((std::is_nothrow_invocable_v<F, Types&&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, false, true, Types>...};

            VISITORS[this->active_index](std::forward<F>(visitor), this->data);

            return true;
        }

        template<typename F>
        requires(std::invocable<F, const Types&> && ...)
        bool Visit(F&& visitor) const& noexcept((std::is_nothrow_invocable_v<F, const Types&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, true, false, Types>...};

            VISITORS[this->active_index](std::forward<F>(visitor), this->data);

            return true;
        }

        template<typename F>
        requires(std::invocable<F, const Types &&> && ...)
        bool Visit(F&& visitor) const&& noexcept((std::is_nothrow_invocable_v<F, const Types&&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, true, true, Types>...};

            VISITORS[this->active_index](std::forward<F>(visitor), this->data);

            return true;
        }
    private:
        alignas(METRICS.alignment) char data[METRICS.size];

        std::conditional_t<
            std::numeric_limits<int8_t>::max() >= sizeof...(Types) - 1,
            int8_t,
            std::conditional_t<std::numeric_limits<int16_t>::max() >= sizeof...(Types) - 1, int16_t, std::conditional_t<std::numeric_limits<int32_t>::max() >= sizeof...(Types) - 1, int32_t, int64_t>>>
            active_index;
    };
};