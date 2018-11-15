#include "stdafx.h"
#include "Camera.h"
#include <algorithm>

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

const float Camera::MinZoom = 0.1f;

Camera::Camera(SGS2D::Viewport* vp) :
    _viewport(vp),
    _position(Vector2(0, 0)),
    _limits(),
    _origin(Vector2(vp->GetWidth() / 2.0f, vp->GetHeight() / 2.0f)),
    _zoom(1.0f),
    _rotation(0.0f),
    _limitSet(false)
{
}

Vector2 Camera::GetPosition()
{
    return _position;
}

Vector2 Camera::GetOrigin()
{
    return _origin;
}

void Camera::SetOrigin(Vector2 origin)
{
    _origin = origin;
}

float Camera::GetZoom()
{
    return _zoom;
}

void Camera::SetZoom(float zoom)
{
    _zoom = std::max(zoom, MinZoom);

    ValidateZoom();
    ValidatePosition();
}

float Camera::GetRotation()
{
    return _rotation;
}

void Camera::SetRotation(float rotation)
{
    _rotation = rotation;
}

SGS2D::Rectangle Camera::GetLimits()
{
    return _limits;
}

void Camera::SetLimits(SGS2D::Rectangle limits)
{
    _limits = limits;
    _limitSet = true;

    ValidateZoom();
    ValidatePosition();
}

Matrix Camera::GetViewMatrix(Vector2 parallax)
{
    return Matrix::CreateTranslation(Vector3(-_position.x * parallax.x, -_position.y * parallax.y, 0.0f)) *
        Matrix::CreateTranslation(Vector3(-_origin.x, -_origin.y, 0.0f)) *
        Matrix::CreateRotationZ(_rotation) *
        Matrix::CreateScale(_zoom, _zoom, 1.0f) *
        Matrix::CreateTranslation(Vector3(_origin.x, _origin.y, 0.0f));
}

void Camera::LookAt(Vector2 position)
{
    SetPosition(position - _origin);
}

void Camera::Move(Vector2 displacement, bool respectRotation)
{
    if (respectRotation)
    {
        displacement = Vector2::Transform(displacement, Matrix::CreateRotationZ(-_rotation));
    }

    SetPosition(_position += displacement);
}

void Camera::Zoom(float delta)
{
    _zoom = std::max(_zoom + delta, MinZoom);

    ValidateZoom();
    ValidatePosition();
}

void Camera::SetPosition(Vector2 position)
{
    _position = position;

    ValidatePosition();
}

void Camera::ValidatePosition()
{
    if (!_limitSet)
    {
        return;
    }

    auto cameraWorldMin = Vector2::Transform(Vector2(0, 0), GetViewMatrix().Invert());
    auto cameraSize = Vector2(_viewport->GetWidth(), _viewport->GetHeight() / _zoom);
    auto limitWorldMin = Vector2(_limits.Left(), _limits.Top());
    auto limitWorldMax = Vector2(_limits.Right(), _limits.Bottom());
    auto positionOffset = _position - cameraWorldMin;

    cameraWorldMin.Clamp(limitWorldMin, limitWorldMax - cameraSize);

    _position = cameraWorldMin + positionOffset;
}

void Camera::ValidateZoom()
{
    if (!_limitSet)
    {
        return;
    }

    auto minZoomX = static_cast<float>(_viewport->GetWidth() / _limits.Width);
    auto minZoomY = static_cast<float>(_viewport->GetHeight() / _limits.Height);

    _zoom =std::max(_zoom, std::max(minZoomX, minZoomY));
}