#pragma once

#include "Singleton.h"
#include "Enemy.h"

namespace SGS2D
{
	class EnemySpawner : public Singleton<EnemySpawner>
	{
	public:
		EnemySpawner();
		void Update();
		void Reset();
	private:
		float _inverseSpawnChance;
		float _inverseBlackHoleSpawnChance;
		DirectX::SimpleMath::Vector2 GetSpawnPosition(Entity::Kind entityKind = Entity::Kind::All);
	};
}