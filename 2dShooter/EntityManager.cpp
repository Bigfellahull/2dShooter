#include "stdafx.h"
#include "EntityManager.h"
#include "SimpleMath.h"
#include "PlayerShip.h"
#include "Enemy.h"
#include "Bullet.h"
#include "EnemySpawner.h"
#include "BlackHole.h"
#include <map>
#include <algorithm>
#include <iterator>

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

void EntityManager::Add(Entity* entity)
{
	if(!_isUpdating)
	{
		AddEntity(entity);
	}
	else
	{
		_addedEntities.push_back(entity);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	_entities.push_back(entity);

	switch(entity->GetKind())
	{
		case Entity::Bullet:
		{
			_bullets.push_back(static_cast<Bullet*>(entity));
			break;
		}
		case Entity::Enemy:
		{
			_enemies.push_back(static_cast<Enemy*>(entity));
			break;
		}
		case Entity::BlackHole:
		{
			_blackholes.push_back(static_cast<BlackHole*>(entity));
			break;
		}
		default:
		{
			break;
		}
	}
}

void EntityManager::Update(InputManager& inputManager, float totalTime, float deltaTime)
{
	_isUpdating = true;

	HandleCollisions(totalTime, deltaTime);

	for(auto entity = _entities.begin(); entity != _entities.end(); ++entity)
	{
        (*entity)->Update(inputManager, totalTime, deltaTime);
        
        if((*entity)->GetIsExpired())
		{
			*entity = nullptr;
		}
	}

	_isUpdating = false;

	for(auto entity = _addedEntities.begin(); entity != _addedEntities.end(); ++entity)
	{
		AddEntity(*entity);
	}

	_addedEntities.clear();

    _entities.erase(std::remove(_entities.begin(), _entities.end(), nullptr), _entities.end());

	// Now check and clear specific entity collections

	for(auto entity = _bullets.begin(); entity != _bullets.end(); ++entity)
	{        
        if((*entity)->GetIsExpired())
		{
			delete *entity;
			*entity = nullptr;
		}
	}

	_bullets.erase(std::remove(_bullets.begin(), _bullets.end(), nullptr), _bullets.end());

	for(auto entity = _enemies.begin(); entity != _enemies.end(); ++entity)
	{        
        if((*entity)->GetIsExpired())
		{
			delete *entity;
			*entity = nullptr;
		}
	}

	_enemies.erase(std::remove(_enemies.begin(), _enemies.end(), nullptr), _enemies.end());

	for(auto entity = _blackholes.begin(); entity != _blackholes.end(); ++entity)
	{        
        if((*entity)->GetIsExpired())
		{
			delete *entity;
			*entity = nullptr;
		}
	}

	_blackholes.erase(std::remove(_blackholes.begin(), _blackholes.end(), nullptr), _blackholes.end());
}

void EntityManager::HandleCollisions(float totalTime, float deltaTime)
{
    for (auto i = 0; i < _enemies.size(); i++)
    {
        auto enemy = _enemies[i];

        for (auto x = i + 1; x < _enemies.size(); x++)
        {
            auto otherEnemy = _enemies[x];

            if (IsColliding(enemy, otherEnemy))
            {
                enemy->HandleCollision(otherEnemy);
                otherEnemy->HandleCollision(enemy);
            }
        }

        for (std::vector<Bullet*>::iterator x = _bullets.begin(); x != _bullets.end(); ++x)
        {
            if (IsColliding(enemy, *x))
            {
                enemy->WasShot();
                (*x)->SetIsExpired(true);
            }
        }

        if (enemy->GetIsActive() && IsColliding(PlayerShip::GetInstance(), enemy))
        {
            KillPlayer(totalTime, deltaTime);
            break;
        }
    }

	for(std::vector<BlackHole*>::iterator i = _blackholes.begin(); i != _blackholes.end(); ++i)
	{
		for(std::vector<Enemy*>::iterator x = _enemies.begin(); x != _enemies.end(); ++x)
		{
			if((*x)->GetIsActive() && IsColliding(*i, *x))
			{
				(*x)->WasShot();
			}
		}

		for(std::vector<Bullet*>::iterator x = _bullets.begin(); x != _bullets.end(); ++x)
		{ 
			if(IsColliding(*i, *x))
			{
                (*i)->WasShot(totalTime, deltaTime);
				(*x)->SetIsExpired(true);
			}
		}

		if(IsColliding(PlayerShip::GetInstance(), *i))
		{
			KillPlayer(totalTime, deltaTime);
			break;
		}
	}
}

void EntityManager::KillPlayer(float totalTime, float deltaTime)
{
	PlayerShip::GetInstance()->Kill();

	for(std::vector<Enemy*>::iterator i = _enemies.begin(); i != _enemies.end(); ++i)
	{
		(*i)->WasShot();
	}

	for(std::vector<BlackHole*>::iterator i = _blackholes.begin(); i != _blackholes.end(); ++i)
	{
		(*i)->Kill(totalTime, deltaTime);
	}

    EnemySpawner::GetInstance()->Reset();
}

int EntityManager::GetEntityCount()
{
	return _entities.size();
}

int EntityManager::GetBlackHoleCount()
{
	return _blackholes.size();
}

std::vector<BlackHole*> EntityManager::GetBlackHoles()
{
    return _blackholes;
}

std::vector<Entity*> EntityManager::GetNearbyEntities(Vector2 position, float radius)
{
	std::vector<Entity*> result;
	for(auto i = _entities.begin(); i != _entities.end(); ++i)
	{
		if(*i)
		{
			if(Vector2::DistanceSquared(position, (*i)->GetPosition()) < radius * radius)
			{
				result.push_back(*i);
			}
		}
	}
	return result;
}

void EntityManager::Draw(SpriteBatch& spriteBatch, float totalTime, float deltaTime)
{
	for (auto entity : _entities)
	{
		entity->Draw(spriteBatch, totalTime, deltaTime);
	}
}

bool EntityManager::IsColliding(Entity* a, Entity* b)
{
    auto radius = a->GetRadius() + b->GetRadius();
    return !a->GetIsExpired() && !b->GetIsExpired() && Vector2::DistanceSquared(a->GetPosition(), b->GetPosition()) < radius * radius;
}