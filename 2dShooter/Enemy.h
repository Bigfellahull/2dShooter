#pragma once

#include "Entity.h"
#include "EntityManager.h"

namespace SGS2D
{
	class Enemy : public Entity
	{
	public:
		enum Behavior
		{
			Follow = 0,
			MoveRandom,
		};

        enum EnemyType
        {
            Follower = 0,
            Wanderer
        };

		Enemy(EnemyType type, Texture2d* texture, DirectX::SimpleMath::Vector2 position);
		void Update(InputManager& inputManager, float totalTime, float deltaTime);
		void Draw(DirectX::SpriteBatch& spriteBatch, float totalTime, float deltaTime);

		bool GetIsActive();
		void WasShot();
        void HandleCollision(Enemy* other);

		static Enemy* CreateSeeker(DirectX::SimpleMath::Vector2 position);
		static Enemy* CreateWanderer(DirectX::SimpleMath::Vector2 position);
	private:
		void AddBehaviour(Behavior behaviour);
		void ApplyBehaviours();
        DirectX::SimpleMath::Vector4 GenerateParticleColour();

		bool FollowPlayer(float acceleration);
		bool MoveRandomly();

		std::vector<Behavior> _behaviours;
		float _randomDirection;
		int _randomState;
		int _pointValue;
		float _timeUntilStart;
        EnemyType _type;
	};
}