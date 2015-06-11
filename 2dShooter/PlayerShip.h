#pragma once

#include "Entity.h"
#include "Singleton.h"
#include "EntityManager.h"
#include "SpriteBatch.h"

namespace SGS2D
{
	class PlayerShip : public Entity, public Singleton<PlayerShip>
	{
	public:
		PlayerShip();
		void Update(InputManager& inputManager, float totalTime, float deltaTime);
        bool IsDead();
        void Kill();
        virtual void Draw(DirectX::SpriteBatch& spriteBatch, float totalTime, float deltaTime);
	private:
        void MakeExhaustFire(float totalTime, float deltaTime);
		int _cooldownFrames;
		int _cooldownRemaining;

        int _framesUntilRespawn;
	};
}