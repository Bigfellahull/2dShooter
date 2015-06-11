#pragma once

#include "Entity.h"
#include "EntityManager.h"

namespace SGS2D
{
	class BlackHole : public Entity
	{
	public:
		BlackHole(DirectX::SimpleMath::Vector2 position);
		void Update(InputManager& inputManager, float totalTime, float deltaTime);
		void Draw(DirectX::SpriteBatch& spriteBatch, float totalTime, float deltaTime);
        void WasShot(float totalTime, float deltaTime);
        void Kill(float totalTime, float deltaTime);
	private:
		int _hitpoints;
        int _pointValue;
        float _sprayAngle;
	};
}