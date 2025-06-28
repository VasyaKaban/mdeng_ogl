#pragma once

#include "matrix.hpp"
#include "vector_view.hpp"

namespace hrs
{
    namespace math
    {
        namespace glsl
        {
            inline mat4x4
            perspective(float fov, float near, float far, float aspect, float min_depth) noexcept
            {
                float half_fov = fov / 2;
                float top = near * std::tan(half_fov);
                float right = top * aspect;

                constexpr float max_depth = 1.0f;

                mat4x4 out;

                out[0][0] = near / right;
                out[1][1] = near / top;
                out[2][2] =
                    ((near - far) * max_depth - (max_depth - min_depth) * near) / (near - far);
                out[3][2] = ((max_depth - min_depth) * far * near) / (near - far);
                out[2][3] = 1.0f;

                return out;
            }

            inline mat4x4 ortho(float left,
                                float right,
                                float bottom,
                                float top,
                                float near,
                                float far,
                                float min_depth) noexcept
            {
                constexpr float max_depth = 1.0f;

                mat4x4 out;

                out[0][0] = 2.0f / (top - bottom);
                out[0][3] = -(top + bottom) / (top - bottom);

                out[1][1] = 2.0f / (right - left);
                out[1][3] = -(right + left) / (right - left);

                out[2][2] = (max_depth - min_depth) / (far - near);
                out[3][2] =
                    (max_depth * (far - near) - far * (max_depth - min_depth)) / (far - near);

                out[3][3] = 1.0f;

                return out;
            }

            inline mat4x4 scale(vec3_view factors) noexcept
            {
                mat4x4 out;
                out[0][0] = factors[0];
                out[1][1] = factors[1];
                out[2][2] = factors[2];
                out[3][3] = 1;

                return out;
            }

            inline mat4x4 translate(vec3_view steps) noexcept
            {
                mat4x4 out = mat4x4::identity();
                out[3][0] = steps[0];
                out[3][1] = steps[1];
                out[3][2] = steps[2];
                out[3][3] = 1;

                return out;
            }
        };
    };
};