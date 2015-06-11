#pragma once

#include "Entity.h"
#include "EntityManager.h"

namespace SGS2D
{
	class Bullet : public Entity
	{
	public:
		Bullet(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 velocity);
		void Update(InputManager& inputManager, float totalTime, float deltaTime);
	};
}