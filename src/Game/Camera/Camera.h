#pragma once

#include "hrs/math/matrix_common.hpp"
#include "Core/Render/Render.h"

class Camera
{
public:
    virtual ~Camera() = 0;

    virtual const hrs::math::glsl::std430::mat4x4& GetMatrix() const noexcept = 0;
    virtual const Render::Viewport& GetViewport() const noexcept = 0;
};

inline Camera::~Camera()
{}