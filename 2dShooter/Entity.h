#pragma once

#include "SimpleMath.h"
#include "Texture2d.h"
#include "DirectXColors.h"
#include "SpriteBatch.h"
#include "GamePad.h"
#include "Art.h"
#include "InputManager.h"

namespace SGS2D
{
	class EntityManager;

	class Entity
	{
	public:
        enum Kind
        {
			All = 0,
            Player,
            Bullet,
            Enemy,
            BlackHole
        };

		Entity(Kind kind);

		DirectX::SimpleMath::Vector4 GetColour();
		void SetColour(DirectX::SimpleMath::Vector4 colour);
		DirectX::SimpleMath::Vector2 GetPosition();
		void SetPosition(DirectX::SimpleMath::Vector2 position);
		DirectX::SimpleMath::Vector2 GetVelocity();
		void SetVelocity(DirectX::SimpleMath::Vector2 velocity);
		float GetOrientation();
		void SetOrientation(float orientation);
		float GetRadius();
		void SetRadius(float radius);
		float GetIsExpired();
		void SetIsExpired(bool expired);
		DirectX::SimpleMath::Vector2 GetSize();
        Kind GetKind();

		virtual void Update(InputManager& inputManager, float totalTime, float deltaTime) = 0;
		virtual void Draw(DirectX::SpriteBatch& spriteBatch, float totalTime, float deltaTime);
	protected:
		Texture2d* _texture;
		DirectX::SimpleMath::Vector4 _colour;
		DirectX::SimpleMath::Vector2 _position;
		DirectX::SimpleMath::Vector2 _velocity;
		float _orientation;
		float _radius;
		bool _isExpired;
        Kind _kind;
	};
}