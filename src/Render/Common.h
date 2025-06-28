#pragma once

#include <glad/gl.h>

using GLHandle = GLuint;

constexpr inline GLuint GL_NULL_HANDLE = 0;

enum class CompareOp
{
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    LessOrEqual = GL_LEQUAL,
    Greater = GL_GREATER,
    NotEqual = GL_NOTEQUAL,
    GreaterOrEqual = GL_GEQUAL,
    Always = GL_ALWAYS
};

enum class SampleCount
{
    SampleCount_1 = 1,
    SampleCount_2 = 2,
    SampleCount_4 = 4,
    SampleCount_8 = 8,
    SampleCount_16 = 16,
    SampleCount_32 = 32,
    SampleCount_64 = 64
};

struct Extent2D
{
    GLsizei width;
    GLsizei height;
};

struct Extent3D
{
    GLsizei width;
    GLsizei height;
    GLsizei depth;
};

struct Offset2D
{
    GLint x;
    GLint y;
};

struct Offset3D
{
    GLint x;
    GLint y;
    GLint z;
};

struct Viewport
{
    float x;
    float y;
    float width;
    float height;
};

struct Rect2D
{
    Offset2D offset;
    Extent2D extent;
};