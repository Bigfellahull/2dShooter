#pragma once

#include "Singleton.h"
#include <vector>
#include "Entity.h"
#include "PlayerShip.h"
#include "Enemy.h"
#include "Bullet.h"
#include "BlackHole.h"

namespace SGS2D
{
	class PlayerShip;
	class Bullet;
	class Enemy;
	class BlackHole;

	class EntityManager : public Singleton<EntityManager>
	{
	public:
		void Add(Entity* entity);
		void Update(InputManager& inputManager, float totalTime, float deltaTime);
		void Draw(DirectX::SpriteBatch& spriteBatch, float totalTime, float deltaTime);
		int GetEntityCount();
		int GetBlackHoleCount();
        std::vector<BlackHole*> GetBlackHoles();
		std::vector<Entity*> GetNearbyEntities(DirectX::SimpleMath::Vector2 position, float radius);
	private:
        void HandleCollisions(float totalTime, float deltaTime);
        bool IsColliding(Entity* a, Entity* b);
        void KillPlayer(float totalTime, float deltaTime);
		void AddEntity(Entity* entity);

		bool _isUpdating;
		std::vector<Entity*> _entities;
		std::vector<Entity*> _addedEntities;
		
		// List that hold the separate entities in order to speed up collision detection
        std::vector<Bullet*> _bullets;
		std::vector<Enemy*> _enemies;
		std::vector<BlackHole*> _blackholes;
	};
}