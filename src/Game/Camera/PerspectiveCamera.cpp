#include "PerspectiveCamera.h"
#include "hrs/math/quaternion.hpp"
#include "hrs/math/glsl.hpp"

PerspectiveCamera::PerspectiveCamera() noexcept
    : offset(0, 0, 0),
      rotation{.yaw = 0, .pitch = 0, .roll = 0},
      perspective{.fov = 0, .aspect = 1, .near = 1, .far = 1},
      updated(false),
      result(decltype(result)::identity(1)),
      view_matrix(decltype(view_matrix)::identity(1)),
      projection_matrix(decltype(projection_matrix)::identity(1))
{}

PerspectiveCamera::~PerspectiveCamera()
{}

void PerspectiveCamera::SignalUpdated() noexcept
{
    updated = true;
}

void PerspectiveCamera::Build() noexcept
{
    if(updated)
    {
        updated = false;
        decltype(view_matrix) offset_matrix = decltype(view_matrix)::identity(1);
        offset_matrix[3] = offset;

        hrs::math::quat q_yaw(hrs::math::glsl::vec3(0, 1, 0), rotation.yaw);
        hrs::math::quat q_pitch(hrs::math::glsl::vec3(1, 0, 0), rotation.pitch);
        hrs::math::quat q_roll(hrs::math::glsl::vec3(0, 0, 1), rotation.roll);

        decltype(view_matrix) rotate_matrix = decltype(view_matrix)::identity(1);
        rotate_matrix = (q_yaw * q_pitch * q_roll).to_matrix();

        view_matrix = offset_matrix * rotate_matrix;

#pragma message("Explicit near_depth = 0!")
        projection_matrix = hrs::math::glsl::perspective(perspective.fov,
                                                         perspective.near,
                                                         perspective.far,
                                                         perspective.aspect,
                                                         0);

        result = view_matrix * projection_matrix;
    }
}

hrs::math::glsl::std430::vec3& PerspectiveCamera::GetOffset() noexcept
{
    return offset;
}

const hrs::math::glsl::std430::vec3& PerspectiveCamera::GetOffset() const noexcept
{
    return offset;
}

RotationParameters& PerspectiveCamera::GetRotation() noexcept
{
    return rotation;
}

const RotationParameters& PerspectiveCamera::GetRotation() const noexcept
{
    return rotation;
}

PerspectiveParameters& PerspectiveCamera::GetPerspective() noexcept
{
    return perspective;
}

const PerspectiveParameters& PerspectiveCamera::GetPerspective() const noexcept
{
    return perspective;
}

Viewport& PerspectiveCamera::GetViewport() noexcept
{
    return viewport;
}

const Viewport& PerspectiveCamera::GetViewport() const noexcept
{
    return viewport;
}

const hrs::math::glsl::std430::mat4x4& PerspectiveCamera::GetViewMatrix() const noexcept
{
    return view_matrix;
}

const hrs::math::glsl::std430::mat4x4& PerspectiveCamera::GetProjectionMatrix() const noexcept
{
    return projection_matrix;
}

const hrs::math::glsl::std430::mat4x4& PerspectiveCamera::GetMatrix() const noexcept
{
    return result;
}