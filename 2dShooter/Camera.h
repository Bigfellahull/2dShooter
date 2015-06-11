#pragma once

#include "SimpleMath.h"
#include "Viewport.h"
#include "Rectangle.h"

namespace SGS2D
{
    class Camera
    {
    public:
        Camera(SGS2D::Viewport* viewport);
        DirectX::SimpleMath::Vector2 GetPosition();
        DirectX::SimpleMath::Vector2 GetOrigin();
        void SetOrigin(DirectX::SimpleMath::Vector2 origin);
        float GetZoom();
        void SetZoom(float zoom);
        float GetRotation();
        void SetRotation(float rotation);
        SGS2D::Rectangle GetLimits();
        void SetLimits(SGS2D::Rectangle limits);
        DirectX::SimpleMath::Matrix GetViewMatrix(DirectX::SimpleMath::Vector2 parallax = DirectX::SimpleMath::Vector2(1, 1));
        void LookAt(DirectX::SimpleMath::Vector2 position);
        void Move(DirectX::SimpleMath::Vector2 displacement, bool respectRotation = false);
        void Zoom(float delta);
    private:
        SGS2D::Viewport* _viewport;
        DirectX::SimpleMath::Vector2 _position;
        SGS2D::Rectangle _limits;
        DirectX::SimpleMath::Vector2 _origin;
        float _zoom;
        float _rotation;
        bool _limitSet;
        void SetPosition(DirectX::SimpleMath::Vector2 position);
        void ValidatePosition();
        void ValidateZoom();
        static const float MinZoom;
    };
}