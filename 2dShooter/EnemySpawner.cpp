#include "stdafx.h"
#include "EnemySpawner.h"
#include "Art.h"
#include "MathHelper.h"
#include "PlayerShip.h"
#include "BlackHole.h"
#include "Viewport.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

EnemySpawner::EnemySpawner() : _inverseSpawnChance(50), _inverseBlackHoleSpawnChance(500)
{
}

void EnemySpawner::Update()
{
	if(!PlayerShip::GetInstance()->IsDead() && EntityManager::GetInstance()->GetEntityCount() < 200)
	{
		if(MathHelper::Random(0, static_cast<int>(_inverseSpawnChance)) == 0)
		{
			EntityManager::GetInstance()->Add(Enemy::CreateSeeker(GetSpawnPosition()));
		}

		if(MathHelper::Random(0, static_cast<int>(_inverseSpawnChance)) == 0)
		{
			EntityManager::GetInstance()->Add(Enemy::CreateWanderer(GetSpawnPosition()));
		}

		if(EntityManager::GetInstance()->GetBlackHoleCount() < 2 && MathHelper::Random(0, static_cast<int>(_inverseBlackHoleSpawnChance)) == 0)
		{
			EntityManager::GetInstance()->Add(new BlackHole(GetSpawnPosition(Entity::BlackHole)));
		}
	}

	if(_inverseSpawnChance > 10)
	{
		_inverseSpawnChance -= 0.005f;
	}

	if(_inverseBlackHoleSpawnChance > 800)
	{
		_inverseBlackHoleSpawnChance -= 0.005f;
	}
}

void EnemySpawner::Reset()
{
	_inverseSpawnChance = 60;
	_inverseBlackHoleSpawnChance = 500;
}

Vector2 EnemySpawner::GetSpawnPosition(Entity::Kind entityKind)
{
	auto xBounds = static_cast<float>(Viewport::GetInstance()->GetWidth()) - (entityKind == Entity::Kind::BlackHole ? 60.0f : 20.0f);
	auto yBounds = static_cast<float>(Viewport::GetInstance()->GetHeight()) - (entityKind == Entity::Kind::BlackHole ? 60.0f : 20.0f);

	Vector2 pos;
	do
	{
        pos = Vector2(MathHelper::Random(0.0f, xBounds), MathHelper::Random(0.0f, yBounds));
	}
	while(Vector2::DistanceSquared(pos, PlayerShip::GetInstance()->GetPosition()) < 300.0f * 300.0f);

	return pos;
}