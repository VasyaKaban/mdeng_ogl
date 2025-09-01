#include "Camera.h"
#include "Core/Render/Common.h"
#include "hrs/math/vector.hpp"
#include "hrs/math/matrix.hpp"

struct RotationParameters
{
    float yaw;
    float pitch;
    float roll;
};

struct PerspectiveParameters
{
    float fov;
    float aspect;
    float near;
    float far;
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera() noexcept;
    virtual ~PerspectiveCamera() override;
    PerspectiveCamera(const PerspectiveCamera&) = default;
    PerspectiveCamera(PerspectiveCamera&&) = default;
    PerspectiveCamera& operator=(const PerspectiveCamera&) = default;
    PerspectiveCamera& operator=(PerspectiveCamera&&) = default;

    void SignalUpdated() noexcept;
    void Build() noexcept;

    hrs::math::glsl::std430::vec3& GetOffset() noexcept;
    const hrs::math::glsl::std430::vec3& GetOffset() const noexcept;

    RotationParameters& GetRotation() noexcept;
    const RotationParameters& GetRotation() const noexcept;

    PerspectiveParameters& GetPerspective() noexcept;
    const PerspectiveParameters& GetPerspective() const noexcept;

    Viewport& GetViewport() noexcept;
    virtual const Viewport& GetViewport() const noexcept override;

    const hrs::math::glsl::std430::mat4x4& GetViewMatrix() const noexcept;
    const hrs::math::glsl::std430::mat4x4& GetProjectionMatrix() const noexcept;

    hrs::math::glsl::std430::vec3
    WindowToWorld(const hrs::math::glsl::vec2_view window_coords) const noexcept;

    virtual const hrs::math::glsl::std430::mat4x4& GetMatrix() const noexcept override;
private:
    //translation
    hrs::math::glsl::std430::vec3 offset;

    //rotation
    RotationParameters rotation;

    //perspective
    PerspectiveParameters perspective;

    //viewport
    Viewport viewport;

    //flag
    bool updated;
    hrs::math::glsl::std430::mat4x4 view_matrix;
    hrs::math::glsl::std430::mat4x4 projection_matrix;
    hrs::math::glsl::std430::mat4x4 result;
};

static_assert(!std::is_abstract_v<PerspectiveCamera>);