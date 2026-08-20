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
            DeviceSize alignment;
            DeviceSize size;
        };

        template<typename T, typename... Types>
        constexpr UnionMetrics GetUnionMetrics() noexcept
        {
            UnionMetrics metrics = {.alignment = Max({alignof(T), alignof(Types)...}), .size = Max({sizeof(T), sizeof(Types)...})};
            Align(metrics.size, metrics.alignment);

            return metrics;
        }

        template<typename T>
        constexpr Void UnionDestructorWrapper(const char* data) noexcept
        {
            reinterpret_cast<const T*>(data)->~T();
        }

        template<typename T>
        constexpr Void UnionCopyConstructorWrapper(char* to, const char* from)
        {
            new(to) T(*reinterpret_cast<const T*>(from));
        }

        template<typename T>
        constexpr Void UnionMoveConstructorWrapper(char* to, char* from)
        {
            new(to) T(Move(*reinterpret_cast<T*>(from)));
        }

        template<typename F, Bool IsConst, Bool IsRVlaue, typename T>
        constexpr Void UnionVisitorWrapper(F&& visitor, const char* data)
        {
            if constexpr(IsConst)
            {
                if constexpr(IsRVlaue)
                    Forward(visitor)(Move(*reinterpret_cast<const T*>(data)));
                else
                    Forward(visitor)(*reinterpret_cast<const T*>(data));
            }
            else
            {
                if constexpr(IsRVlaue)
                    Forward(visitor)(Move(*reinterpret_cast<T*>(const_cast<char*>(data))));
                else
                    Forward(visitor)(*reinterpret_cast<T*>(const_cast<char*>(data)));
            }
        }
    };

    constexpr inline Int64 NON_ACTIVE_UNION_INDEX = -1;

    template<typename... Types>
    requires(sizeof...(Types) > 0 && (sizeof...(Types) - 1 <= NumericLimits<Int64>::Max)) && (SameAs<DropConstVolatileReference<Types>, Types> && ...)
    class Union
    {
        constexpr static Detail::UnionMetrics METRICS = Detail::GetUnionMetrics<Types...>();
        using VariadicTypes = Variadic<Types...>;
    public:
        template<DeviceSize Index>
        requires(Index < sizeof...(Types))
        using TypeOfIndex = typename VariadicTypes::template TypeOfIndex<Index>;

        template<typename T>
        requires(SameAs<T, Types> || ...)
        constexpr static DeviceSize IndexOfType = VariadicTypes::template IndexOfType<T>;

        Union() noexcept
            : active_index(NON_ACTIVE_UNION_INDEX)
        {}

        template<Integral auto Index, typename... Args>
        requires(Index >= 0 && Index < sizeof...(Types)) && Constructible<TypeOfIndex<Index>, Args...>
        Union(NonType<Index>, Args&&... args) noexcept(NoexceptConstructible<TypeOfIndex<Index>, Args...>)
        {
            new(this->data) TypeOfIndex<Index>(Forward(args)...);

            this->active_index = Index;
        }

        template<typename T, typename... Args>
        requires(SameAs<T, Types> || ...)
        Union(InPlaceType<T>, Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
        {
            new(this->data) T(Forward(args)...);

            this->active_index = IndexOfType<T>;
        }

        ~Union()
        {
            constexpr static Void (*DESTRUCTORS[])(const char* data) = {&Detail::UnionDestructorWrapper<Types>...};

            if(this->active_index != NON_ACTIVE_UNION_INDEX)
                DESTRUCTORS[this->active_index](this->data);
        }

        Union(const Union& un) noexcept((NoexceptCopyConstructible<Types> && ...))
        requires(CopyConstructible<Types> && ...)
        {
            constexpr static Void (*COPY_CONSTRUCTORS[])(char* to, const char* from) = {&Detail::UnionCopyConstructorWrapper<Types>...};

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                COPY_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;
        }

        Union(Union&& un) noexcept((NoexceptMoveConstructible<Types> && ...))
        requires(MoveConstructible<Types> && ...)
        {
            constexpr static Void (*MOVE_CONSTRUCTORS[])(char* to, char* from) = {&Detail::UnionMoveConstructorWrapper<Types>...};

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                MOVE_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;
        }

        Union& operator=(const Union& un) noexcept((NoexceptCopyConstructible<Types> && ...))
        requires(CopyConstructible<Types> && ...)
        {
            constexpr static Void (*COPY_CONSTRUCTORS[])(char* to, const char* from) = {&Detail::UnionCopyConstructorWrapper<Types>...};

            this->Reset();

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                COPY_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;

            return *this;
        }

        Union& operator=(Union&& un) noexcept((NoexceptMoveConstructible<Types> && ...))
        requires(MoveConstructible<Types> && ...)
        {
            constexpr static Void (*MOVE_CONSTRUCTORS[])(char* to, char* from) = {&Detail::UnionMoveConstructorWrapper<Types>...};

            this->Reset();

            if(un.GetActiveIndex() != NON_ACTIVE_UNION_INDEX)
            {
                MOVE_CONSTRUCTORS[un.GetActiveIndex()](this->data, un.data);
            }

            this->active_index = un.active_index;

            return *this;
        }

        decltype(NON_ACTIVE_UNION_INDEX) GetActiveIndex() const noexcept
        {
            return this->active_index;
        }

        Void Reset() noexcept
        {
            if(this->active_index != NON_ACTIVE_UNION_INDEX)
            {
                this->~Union();

                this->active_index = NON_ACTIVE_UNION_INDEX;
            }
        }

        template<typename T>
        requires(SameAs<T, Types> || ...)
        Bool IsActive() const noexcept
        {
            return this->active_index == IndexOfType<T>;
        }

        //Get by Index
        template<DeviceSize Index>
        requires(Index < sizeof...(Types))
        TypeOfIndex<Index>& Get() & noexcept
        {
            assert(this->active_index == Index);

            return *reinterpret_cast<TypeOfIndex<Index>*>(this->data);
        }

        template<DeviceSize Index>
        requires(Index < sizeof...(Types))
        const TypeOfIndex<Index>& Get() const& noexcept
        {
            assert(this->active_index == Index);

            return *reinterpret_cast<const TypeOfIndex<Index>*>(this->data);
        }

        template<DeviceSize Index>
        requires(Index < sizeof...(Types))
        TypeOfIndex<Index>&& Get() && noexcept
        {
            assert(this->active_index == Index);

            return Move(*reinterpret_cast<TypeOfIndex<Index>*>(this->data));
        }

        template<DeviceSize Index>
        requires(Index < sizeof...(Types))
        const TypeOfIndex<Index>&& Get() const&& noexcept
        {
            assert(this->active_index == Index);

            return Move(*reinterpret_cast<const TypeOfIndex<Index>*>(this->data));
        }

        //Get by Type
        template<typename T>
        requires(SameAs<T, Types> || ...)
        T& Get() noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return *reinterpret_cast<T*>(this->data);
        }

        template<typename T>
        requires(SameAs<T, Types> || ...)
        const T& Get() const& noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return *reinterpret_cast<const T*>(this->data);
        }

        template<typename T>
        requires(SameAs<T, Types> || ...)
        T&& Get() && noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return Move(*reinterpret_cast<T*>(this->data));
        }

        template<typename T>
        requires(SameAs<T, Types> || ...)
        const T&& Get() const&& noexcept
        {
            assert(this->active_index == IndexOfType<T>);

            return Move(*reinterpret_cast<const T*>(this->data));
        }

        //Set by Index
        template<DeviceSize Index, typename... Args>
        requires(Index < sizeof...(Types)) && Constructible<TypeOfIndex<Index>, Args...>
        Void Set(Args&&... args) noexcept(NoexceptConstructible<TypeOfIndex<Index>, Args...>)
        {
            this->Reset();

            new(this->data) TypeOfIndex<Index>(Forward(args)...);

            this->active_index = Index;
        }

        //Set by Type
        template<typename T, typename... Args>
        requires(SameAs<T, Types> || ...) && Constructible<T, Args...>
        Void Set(Args&&... args) noexcept(NoexceptConstructible<T, Args...>)
        {
            this->Reset();

            new(this->data) T(Forward(args)...);

            this->active_index = IndexOfType<T>;
        }

        template<typename F>
        requires(Invocable<F, Types&> && ...)
        Bool Visit(F&& visitor) & noexcept((NoexceptInvocable<F, Types&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static Void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, false, false, Types>...};

            VISITORS[this->active_index](Forward(visitor), this->data);

            return true;
        }

        template<typename F>
        requires(Invocable<F, Types &&> && ...)
        Bool Visit(F&& visitor) && noexcept((NoexceptInvocable<F, Types&&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static Void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, false, true, Types>...};

            VISITORS[this->active_index](Forward(visitor), this->data);

            return true;
        }

        template<typename F>
        requires(Invocable<F, const Types&> && ...)
        Bool Visit(F&& visitor) const& noexcept((NoexceptInvocable<F, const Types&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static Void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, true, false, Types>...};

            VISITORS[this->active_index](Forward(visitor), this->data);

            return true;
        }

        template<typename F>
        requires(Invocable<F, const Types &&> && ...)
        Bool Visit(F&& visitor) const&& noexcept((NoexceptInvocable<F, const Types&&> && ...))
        {
            if(this->active_index == NON_ACTIVE_UNION_INDEX)
                return false;

            constexpr static Void (*VISITORS[])(F&& visitor, const char* data) = {&Detail::UnionVisitorWrapper<F, true, true, Types>...};

            VISITORS[this->active_index](Forward(visitor), this->data);

            return true;
        }
    private:
        alignas(METRICS.alignment) UInt8 data[METRICS.size];

        Conditional<NumericLimits<Int8>::Max >= sizeof...(Types) - 1,
                    Int8,
                    Conditional<NumericLimits<Int16>::Max >= sizeof...(Types) - 1, Int16, Conditional<NumericLimits<Int32>::Max >= sizeof...(Types) - 1, Int32, Int64>>>
            active_index;
    };
};