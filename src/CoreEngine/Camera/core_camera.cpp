#include <CoreEngine/Camera/core_camera.h>

#include <array>

#include <Math/math.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <CoreEngine/engine.h>

namespace poke {
CoreCamera::CoreCamera(Engine& engine)
    : fieldOfView_(70.0f),
      nearPlane_(0.1f),
      farPlane_(1000.0f)
{
    position_ = math::Vec3(0.0f, 0.0f, -5.0f);
    front_ = {0, 0, -1};
}

void CoreCamera::Update()
{
    auto& window = GraphicsEngineLocator::Get().GetWindow();

    viewMatrix_ = math::Matrix4::ViewMatrix(position_, rotation_);
    projectionMatrix_ = math::Matrix4::Perspective(
        GetFieldOfView(),
        static_cast<float>(window.GetSize().x) / static_cast<float>(window.GetSize().y),
        nearPlane_,
        farPlane_);

    const auto invertedView = math::Matrix4::GetInverse(viewMatrix_);
	const auto f = invertedView.GetColumn(2).Normalize();
	front_ = { f.x, f.y, f.z };
	const auto r = invertedView.GetColumn(0).Normalize();
	right_ = { r.x, r.y, r.z };
	const auto u = invertedView.GetColumn(1).Normalize();
	up_ = { u.x, u.y, u.z };
}

math::Matrix4 CoreCamera::GetProjectionMatrix() const { return projectionMatrix_; }

void CoreCamera::SetProjectionMatrix(const math::Matrix4 projectionMatrix)
{
    projectionMatrix_ = projectionMatrix;
}

math::Matrix4 CoreCamera::GetViewMatrix() const { return viewMatrix_; }

void CoreCamera::SetViewMatrix(const math::Matrix4 viewMatrix)
{
    viewMatrix_ = viewMatrix;

    const auto invertedView = math::Matrix4::GetInverse(viewMatrix_);
    const auto f = invertedView.GetColumn(2).Normalize();
    front_ = {f.x, f.y, f.z};
    const auto r = invertedView.GetColumn(0).Normalize();
    right_ = {r.x, r.y, r.z};
    const auto u = invertedView.GetColumn(1).Normalize();
    up_ = {u.x, u.y, u.z};

    position_ = {invertedView[3][0], invertedView[3][1], invertedView[3][2]};

    rotation_ = math::Vec3(
        atan2f(invertedView[1][2], invertedView[2][2]),
        atan2f(
            -invertedView[0][2],
            sqrtf(
                invertedView[1][2] * invertedView[1][2] + invertedView[2][2] * invertedView[2][2])),
        atan2f(invertedView[0][1], invertedView[0][0]));
}

math::Vec3 CoreCamera::GetPosition() const { return position_; }

void CoreCamera::SetPosition(const math::Vec3 newPosition) { position_ = newPosition; }

math::Vec3 CoreCamera::GetRotation() const { return rotation_; }

void CoreCamera::SetRotation(const math::Vec3 rotation) { rotation_ = rotation; }

math::Vec3 CoreCamera::GetVelocity() const { return velocity_; }

void CoreCamera::SetVelocity(const math::Vec3 velocity) { velocity_ = velocity; }

float CoreCamera::GetFieldOfView() const { return fieldOfView_; }

void CoreCamera::SetFieldOfView(const float fieldOfView) { fieldOfView_ = fieldOfView; }

float CoreCamera::GetNearPlan() const { return nearPlane_; }

void CoreCamera::SetNearPlan(const float nearPlan) { nearPlane_ = nearPlan; }

float CoreCamera::GetFarPlan() const { return farPlane_; }

void CoreCamera::SetFarPlan(const float farPlan) { farPlane_ = farPlan; }

FrustumPlanes CoreCamera::GetFrustumPlanes()
{
    std::array<float, 16> clip{};

    clip[0] = viewMatrix_[0][0] * projectionMatrix_[0][0] + viewMatrix_[0][1] *
              projectionMatrix_[1][0] + viewMatrix_[0][2] * projectionMatrix_[2]
              [0] + viewMatrix_[0][3] * projectionMatrix_[3][0];
    clip[1] = viewMatrix_[0][0] * projectionMatrix_[0][1] + viewMatrix_[0][1] *
              projectionMatrix_[1][1] + viewMatrix_[0][2] * projectionMatrix_[2]
              [1] + viewMatrix_[0][3] * projectionMatrix_[3][1];
    clip[2] = viewMatrix_[0][0] * projectionMatrix_[0][2] + viewMatrix_[0][1] *
              projectionMatrix_[1][2] + viewMatrix_[0][2] * projectionMatrix_[2]
              [2] + viewMatrix_[0][3] * projectionMatrix_[3][2];
    clip[3] = viewMatrix_[0][0] * projectionMatrix_[0][3] + viewMatrix_[0][1] *
              projectionMatrix_[1][3] + viewMatrix_[0][2] * projectionMatrix_[2]
              [3] + viewMatrix_[0][3] * projectionMatrix_[3][3];

    clip[4] = viewMatrix_[1][0] * projectionMatrix_[0][0] + viewMatrix_[1][1] *
              projectionMatrix_[1][0] + viewMatrix_[1][2] * projectionMatrix_[2]
              [0] + viewMatrix_[1][3] * projectionMatrix_[3][0];
    clip[5] = viewMatrix_[1][0] * projectionMatrix_[0][1] + viewMatrix_[1][1] *
              projectionMatrix_[1][1] + viewMatrix_[1][2] * projectionMatrix_[2]
              [1] + viewMatrix_[1][3] * projectionMatrix_[3][1];
    clip[6] = viewMatrix_[1][0] * projectionMatrix_[0][2] + viewMatrix_[1][1] *
              projectionMatrix_[1][2] + viewMatrix_[1][2] * projectionMatrix_[2]
              [2] + viewMatrix_[1][3] * projectionMatrix_[3][2];
    clip[7] = viewMatrix_[1][0] * projectionMatrix_[0][3] + viewMatrix_[1][1] *
              projectionMatrix_[1][3] + viewMatrix_[1][2] * projectionMatrix_[2]
              [3] + viewMatrix_[1][3] * projectionMatrix_[3][3];

    clip[8] = viewMatrix_[2][0] * projectionMatrix_[0][0] + viewMatrix_[2][1] *
              projectionMatrix_[1][0] + viewMatrix_[2][2] * projectionMatrix_[2]
              [0] + viewMatrix_[2][3] * projectionMatrix_[3][0];
    clip[9] = viewMatrix_[2][0] * projectionMatrix_[0][1] + viewMatrix_[2][1] *
              projectionMatrix_[1][1] + viewMatrix_[2][2] * projectionMatrix_[2]
              [1] + viewMatrix_[2][3] * projectionMatrix_[3][1];
    clip[10] = viewMatrix_[2][0] * projectionMatrix_[0][2] + viewMatrix_[2][1] *
               projectionMatrix_[1][2] + viewMatrix_[2][2] * projectionMatrix_[2
               ][2] + viewMatrix_[2][3] * projectionMatrix_[3][2];
    clip[11] = viewMatrix_[2][0] * projectionMatrix_[0][3] + viewMatrix_[2][1] *
               projectionMatrix_[1][3] + viewMatrix_[2][2] * projectionMatrix_[2
               ][3] + viewMatrix_[2][3] * projectionMatrix_[3][3];

    clip[12] = viewMatrix_[3][0] * projectionMatrix_[0][0] + viewMatrix_[3][1] *
               projectionMatrix_[1][0] + viewMatrix_[3][2] * projectionMatrix_[2
               ][0] + viewMatrix_[3][3] * projectionMatrix_[3][0];
    clip[13] = viewMatrix_[3][0] * projectionMatrix_[0][1] + viewMatrix_[3][1] *
               projectionMatrix_[1][1] + viewMatrix_[3][2] * projectionMatrix_[2
               ][1] + viewMatrix_[3][3] * projectionMatrix_[3][1];
    clip[14] = viewMatrix_[3][0] * projectionMatrix_[0][2] + viewMatrix_[3][1] *
               projectionMatrix_[1][2] + viewMatrix_[3][2] * projectionMatrix_[2
               ][2] + viewMatrix_[3][3] * projectionMatrix_[3][2];
    clip[15] = viewMatrix_[3][0] * projectionMatrix_[0][3] + viewMatrix_[3][1] *
               projectionMatrix_[1][3] + viewMatrix_[3][2] * projectionMatrix_[2
               ][3] + viewMatrix_[3][3] * projectionMatrix_[3][3];

    std::array<math::Vec4, 6> frustum;

    // This will extract the LEFT side of the frustum.
    frustum[1][0] = clip[3] - clip[0];
    frustum[1][1] = clip[7] - clip[4];
    frustum[1][2] = clip[11] - clip[8];
    frustum[1][3] = clip[15] - clip[12];

    // This will extract the RIGHT side of the frustum.
    frustum[0][0] = clip[3] + clip[0];
    frustum[0][1] = clip[7] + clip[4];
    frustum[0][2] = clip[11] + clip[8];
    frustum[0][3] = clip[15] + clip[12];

    // This will extract the BOTTOM side of the frustum.
    frustum[2][0] = clip[3] + clip[1];
    frustum[2][1] = clip[7] + clip[5];
    frustum[2][2] = clip[11] + clip[9];
    frustum[2][3] = clip[15] + clip[13];

    // This will extract the TOP side of the frustum.
    frustum[3][0] = clip[3] - clip[1];
    frustum[3][1] = clip[7] - clip[5];
    frustum[3][2] = clip[11] - clip[9];
    frustum[3][3] = clip[15] - clip[13];

    // This will extract the BACK side of the frustum.
    frustum[4][0] = clip[3] + clip[2];
    frustum[4][1] = clip[7] + clip[6];
    frustum[4][2] = clip[11] + clip[10];
    frustum[4][3] = clip[15] + clip[14];

    // This will extract the FRONT side of the frustum.
    frustum[5][0] = clip[3] - clip[2];
    frustum[5][1] = clip[7] - clip[6];
    frustum[5][2] = clip[11] - clip[10];
    frustum[5][3] = clip[15] - clip[14];

    for (int i = 0; i < 6; i++) {
        const auto magnitude = std::sqrt(
            frustum[i][0] * frustum[i][0] + frustum[i][1] * frustum[i][1] +
            frustum[i][2] * frustum[i][2]);
        frustum[i][0] /= magnitude;
        frustum[i][1] /= magnitude;
        frustum[i][2] /= magnitude;
        frustum[i][3] /= magnitude;
    }
    return frustum;
}
} //namespace poke
