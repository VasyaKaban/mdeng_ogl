#pragma once

#include <compare>
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
        constexpr void CollectUnionMetrics(UnionMetrics& metrics) noexcept
        {
            if(alignof(T) > metrics.alignment)
                metrics.alignment = alignof(T);

            if(sizeof(T) > metrics.size)
                metrics.size = sizeof(T);

            if constexpr(sizeof...(Types) != 0)
                CollectUnionMetrics<Types...>(metrics);
        }

        template<typename T, typename... Types>
        constexpr UnionMetrics GetUnionMetrics() noexcept
        {
            UnionMetrics metrics{.alignment = 0, .size = 0};
            CollectUnionMetrics<T, Types...>(metrics);

            Align(metrics.size, metrics.alignment);

            return metrics;
        }
    };

    constexpr inline ptrdiff_t INACTIVE_UNION_INDEX = -1;

    template<typename... Types>
    requires(sizeof...(Types) != 0) && (std::same_as<std::remove_cvref_t<Types>, Types> && ...)
    class Union
    {
        constexpr static auto METRICS = Detail::GetUnionMetrics<Types...>();
    public:
        using Variadic = Variadic<Types...>;

        Union() noexcept
            : active_index(INACTIVE_UNION_INDEX)
        {}

        ~Union()
        {
            if(active_index != INACTIVE_UNION_INDEX)
                Destroy<0, Types...>();
        }

        Union(const Union& un)
        requires(std::copy_constructible<Types> && ...)
            : Union()
        {
            if(un.active_index != INACTIVE_UNION_INDEX)
                CopyConstruct<0, Types...>(un);
        }

        Union(Union&& un)
        requires(std::move_constructible<Types> && ...)
            : Union()
        {
            if(un.active_index != INACTIVE_UNION_INDEX)
                CopyConstruct<0, Types...>(std::move(un));
        }

        /////////////////////////////////////////////////////////////////////////////////
        Union& operator=(const Union& un);
        Union& operator=(Union&& un);
        /////////////////////////////////////////////////////////////////////////////////

        template<typename U>
        requires(std::constructible_from<Types, U> || ...)
        Union(U&& value)
            : Union()
        {
            ConstructFrom<0, Types...>(std::forward<U>(value));
        }

        /////////////////////////////////////////////////////////////////////////////////
        template<typename U>
        requires(std::constructible_from<Types, U> || ...)
        Union& operator=(U&& value);
        ////////////////////////////////////////////////////////////////////////////////

        void Reset() noexcept
        {
            if(this->active_index != INACTIVE_UNION_INDEX)
            {
                Destroy<0, Types...>();
                this->active_index = INACTIVE_UNION_INDEX;
            }
        }

        ptrdiff_t GetActiveIndex() const noexcept
        {
            return this->active_index;
        }

        bool IsInactive() const noexcept
        {
            return this->active_index == INACTIVE_UNION_INDEX;
        }

        template<typename U>
        requires(std::same_as<Types, U> || ...)
        bool Holds() const noexcept
        {
            return HoldsType<U, Types...>();
        }

        //no volatile, & and && -> do not care until we meet them :)
        template<typename F>
        requires(std::invocable<F, Types> && ...)
        void Visit(F&& visitor) noexcept(noexcept((std::forward<F>(visitor)(*reinterpret_cast<Types*>(this->data)) && ...)))
        {
            if(this->active_index != INACTIVE_UNION_INDEX)
                VisitTypes<noexcept((std::forward<F>(visitor)(*reinterpret_cast<Types*>(this->data)) && ...)), 0, Types...>(std::forward<F>(visitor));
        }

        template<typename F>
        requires(std::invocable<F, const Types> && ...)
        void Visit(F&& visitor) const noexcept(noexcept((std::forward<F>(visitor)(*reinterpret_cast<const Types*>(this->data)) && ...)))
        {
            if(this->active_index != INACTIVE_UNION_INDEX)
                VisitTypes<noexcept((std::forward<F>(visitor)(*reinterpret_cast<const Types*>(this->data)) && ...)), 0, Types...>(std::forward<F>(visitor));
        }

        ////////////////////////////////////////////////////////////////////////////////
        std::common_comparison_category_t<std::compare_three_way_result_t<Types>...> operator<=>(const Union& un) const
            noexcept(noexcept(((*reinterpret_cast<const Types*>(this->data) <=> *reinterpret_cast<const Types*>(this->data)) && ...)));
        ////////////////////////////////////////////////////////////////////////////////
    private:
        template<size_t Index, typename NT, typename... NTypes>
        void Destroy() noexcept
        {
            if(Index == this->active_index)
            {
                reinterpret_cast<const NT*>(this->data)->~NT();
            }

            if constexpr(sizeof...(NTypes) != 0)
                Destroy<Index + 1, NTypes...>();
        }

        template<typename U, typename NT, typename... NTypes>
        bool HoldsType() const noexcept
        {
            if constexpr(std::same_as<NT, U>)
                return true;
            else if constexpr(sizeof...(Types) == 0)
                return false;
            else
                return HoldsType<U, NTypes...>();
        }

        template<size_t Index, typename NT, typename... NTypes>
        void CopyConstruct(const Union& un)
        {
            if(Index == un.active_index)
            {
                new(this->data) NT(*reinterpret_cast<const NT*>(un.data));
                this->active_index = un.active_index;
            }
            else
                CopyConstruct<Index + 1, NTypes...>(un);
        }

        template<size_t Index, typename NT, typename... NTypes>
        void MoveConstruct(Union&& un)
        {
            if(Index == un.active_index)
            {
                new(this->data) NT(std::move(*reinterpret_cast<NT*>(un.data)));
                this->active_index = un.active_index;
            }
            else
                MoveConstruct<Index + 1, NTypes...>(std::move(un));
        }

        template<size_t Index, typename NT, typename... NTypes, typename U>
        void ConstructFrom(U&& value)
        {
            if(std::constructible_from<NT, U>)
                new(this->data) NT(std::forward<U>(value));
            else if constexpr(sizeof...(NTypes) != 0)
                ConstructFrom<Index + 1, NTypes...>(std::forward<U>(value));
        }

        template<bool IsNoexcept, size_t Index, typename NT, typename... NTypes, typename F>
        void VisitTypes(F&& visitor) noexcept(IsNoexcept)
        {
            if(Index == this->active_index)
                std::forward<F>(visitor)(*reinterpret_cast<NT*>(this->data));
            else
                VisitTypes<IsNoexcept, Index + 1, NTypes...>(std::forward<F>(visitor));
        }

        template<bool IsNoexcept, size_t Index, typename NT, typename... NTypes, typename F>
        void VisitTypes(F&& visitor) const noexcept(IsNoexcept)
        {
            if(Index == this->active_index)
                std::forward<F>(visitor)(*reinterpret_cast<const NT*>(this->data));
            else
                VisitTypes<IsNoexcept, Index + 1, NTypes...>(std::forward<F>(visitor));
        }
    private:
        alignas(METRICS.alignment) char data[METRICS.size];
        ptrdiff_t active_index; //-1 -> no active
    };
};

void foo()
{
#error CREATE TARGET TYPE INSTEAD OF CONVERTING + Get and Visit ad free functions???
    Core::Union<int, float, char> un('d');
    un.Holds<char>();
}

//none
//const
//&
//&&
//const &
//const &&