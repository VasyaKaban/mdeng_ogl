#pragma once

#define CORE_NON_COPY_CONSTRUCTIBLE(CLASS_NAME) CLASS_NAME(const CLASS_NAME&) = delete;
#define CORE_NON_COPY_ASSIGNABLE(CLASS_NAME) CLASS_NAME& operator=(const CLASS_NAME&) = delete;

#define CORE_NON_MOVE_CONSTRUCTIBLE(CLASS_NAME) CLASS_NAME(CLASS_NAME&&) = delete;
#define CORE_NON_MOVE_ASSIGNABLE(CLASS_NAME) CLASS_NAME& operator=(CLASS_NAME&&) = delete;

#define CORE_NON_COPYABLE(CLASS_NAME) \
    CORE_NON_COPY_CONSTRUCTIBLE(CLASS_NAME) \
    CORE_NON_COPY_ASSIGNABLE(CLASS_NAME)

#define CORE_NON_MOVABLE(CLASS_NAME) \
    CORE_NON_MOVE_CONSTRUCTIBLE(CLASS_NAME) \
    CORE_NON_MOVE_ASSIGNABLE(CLASS_NAME)

#define CORE_NON_CREATABLE(CLASS_NAME) \
    CLASS_NAME() = delete; \
    CORE_NON_COPYABLE(CLASS_NAME) \
    CORE_NON_MOVABLE(CLASS_NAME)

#include <utility>

namespace Core
{

    struct NonCopyConstructible
    {
        NonCopyConstructible() = default;
        NonCopyConstructible(const NonCopyConstructible&) = delete;
        NonCopyConstructible(NonCopyConstructible&&) = default;
        NonCopyConstructible& operator=(const NonCopyConstructible&) = default;
        NonCopyConstructible& operator=(NonCopyConstructible&&) = default;
    };

    struct NonMoveConstructible
    {
        NonMoveConstructible() = default;
        NonMoveConstructible(const NonMoveConstructible&) = default;
        NonMoveConstructible(NonMoveConstructible&&) = delete;
        NonMoveConstructible& operator=(const NonMoveConstructible&) = default;
        NonMoveConstructible& operator=(NonMoveConstructible&&) = default;
    };

    struct NonCopyAssignable
    {
        NonCopyAssignable() = default;
        NonCopyAssignable(const NonCopyAssignable&) = default;
        NonCopyAssignable(NonCopyAssignable&&) = default;
        NonCopyAssignable& operator=(const NonCopyAssignable&) = delete;
        NonCopyAssignable& operator=(NonCopyAssignable&&) = default;
    };

    struct NonMoveAssignable
    {
        NonMoveAssignable() = default;
        NonMoveAssignable(const NonMoveAssignable&) = default;
        NonMoveAssignable(NonMoveAssignable&&) = default;
        NonMoveAssignable& operator=(const NonMoveAssignable&) = default;
        NonMoveAssignable& operator=(NonMoveAssignable&&) = delete;
    };

    struct NonCopyable : NonCopyConstructible, NonCopyAssignable
    {};

    struct NonMovable : NonMoveConstructible, NonMoveAssignable
    {};

    struct NonCreatable : NonCopyable, NonMovable
    {
        NonCreatable() = delete;
    };
};
