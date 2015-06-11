#include "stdafx.h"
#include "Enemy.h"
#include "Art.h"
#include "PlayerShip.h"
#include "MathHelper.h"
#include "Rectangle.h"
#include "PlayerStatus.h"
#include "ParticleManager.h"
#include "Viewport.h"
#include "ColourUtility.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy(EnemyType type, Texture2d* texture, Vector2 position) : 
    _type(type),
    _timeUntilStart(60),
    Entity(Entity::Kind::Enemy)
{
	_texture = texture;
	_position = position;
	_radius = texture->GetWidth() / 2.0f;
}

void Enemy::AddBehaviour(Behavior behavior)
{
	_behaviours.push_back(behavior);
}

void Enemy::ApplyBehaviours()
{
	std::vector<Behavior>::iterator iter, iterNext;

    iter = _behaviours.begin();
    iterNext = iter;

    while (iter != _behaviours.end())
    {
        iterNext++;

        bool result = false;

        switch (*iter)
        {
			case Follow: result = FollowPlayer(1.2f); break;
            case MoveRandom: result = MoveRandomly(); break;
        }

        if (!result)
        {
            _behaviours.erase(iter);
        }

        iter = iterNext;
    }
}

void Enemy::Update(InputManager& inputManager, float totalTime, float deltaTime)
{
	auto viewport = Viewport::GetInstance();

	if(_timeUntilStart <= 0)
	{
		ApplyBehaviours();
	}
	else
	{
		_timeUntilStart--;
		_colour = Colors::White * (1.0f - static_cast<float>(_timeUntilStart) / 60.0f);
	}

	_position += _velocity;
	
    auto size = GetSize();
    _position.Clamp(size, (Vector2(viewport->GetWidth(), viewport->GetHeight()) - size));

	_velocity *= 0.85f;
}

void Enemy::Draw(SpriteBatch& spriteBatch, float totalTime, float deltaTime)
{
	auto size = GetSize();

	if(_timeUntilStart > 0)
	{
        if (_orientation == 0.0f) {
            _orientation = MathHelper::Random(0.0f, 6.2f);
        }

		auto factor = _timeUntilStart / 60.f;
        spriteBatch.Draw(_texture->GetSrv(), _position, nullptr, Colors::White * (1.0f - static_cast<float>(_timeUntilStart) / 60.0f), _orientation, Vector2(size.x / 2, size.y / 2), 1.6f - factor, SpriteEffects::SpriteEffects_None, 0);
	}

	Entity::Draw(spriteBatch, totalTime, deltaTime);
}

void Enemy::HandleCollision(Enemy* other)
{
    auto distance = _position - other->GetPosition();
    _velocity += 10.0f * distance / (distance.LengthSquared() + 1);
}

void Enemy::WasShot()
{
	_isExpired = true;

	PlayerStatus::GetInstance()->AddPoints(_pointValue);
	PlayerStatus::GetInstance()->IncreaseMultiplier();

    auto particleColour = GenerateParticleColour();

	for(auto i = 0; i < 150; i++)
	{
		auto speed = 11.0f * (1.0f - 1 / MathHelper::Random(1.0f, 10.0f));
		
        auto velocity = MathHelper::NextVector2(speed, speed);
                
        ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), _position, velocity, particleColour, 230.0f, Vector2(0.7f, 0.7f));
	}
}

Vector4 Enemy::GenerateParticleColour()
{
    auto hue1 = MathHelper::Random(0.0f, 6.0f);
    auto hue2 = std::fmod(hue1 + MathHelper::Random(0.0f, 2.0f), 6.0f);
	
	auto colour1 = ColourUtility::HsvToColour(hue1, 0.8f, 1.0f);
	auto colour2 = ColourUtility::HsvToColour(hue2, 0.8f, 1.0f);
	
	return Vector4::Lerp(colour1, colour2, MathHelper::Random(0.0f, 1.0f));
}

bool Enemy::GetIsActive()
{
	return _timeUntilStart <= 0;
}

Enemy* Enemy::CreateSeeker(Vector2 position)
{
	auto enemy = new Enemy(EnemyType::Follower, Art::GetInstance()->GetSeeker(), position);

	enemy->_pointValue = 2;
	enemy->AddBehaviour(Follow);

	return enemy;
}

Enemy* Enemy::CreateWanderer(Vector2 position)
{
	auto enemy = new Enemy(EnemyType::Wanderer, Art::GetInstance()->GetWanderer(), position);

	enemy->_randomDirection = MathHelper::Random(0.0f, 1.0f) * M_PI * 2.0f;
	enemy->_randomState = 0;

	enemy->_pointValue = 1;
	enemy->AddBehaviour(MoveRandom);

	return enemy;
}

bool Enemy::FollowPlayer(float acceleration)
{
	auto tempVelocity = (PlayerShip::GetInstance()->GetPosition() - _position);
		
	tempVelocity.Normalize();
		
	tempVelocity *= acceleration;
	_velocity += tempVelocity;
    
	if(_velocity != Vector2::Zero)
	{
		_orientation = static_cast<float>(std::atan2(_velocity.y, _velocity.x));
	}

	return true;
}

bool Enemy::MoveRandomly()
{
	if(_randomState == 0)
	{
		_randomDirection += MathHelper::Random(0.0f, 1.0f) * 0.2f - 0.1f;
	}

	_velocity += 0.4f * Vector2(std::cosf(_randomDirection), sinf(_randomDirection));
	_orientation -= 0.05f;

	auto viewport = Viewport::GetInstance();
	auto bounds = viewport->GetBounds();
    
    bounds.X += GetSize().x + 4.0f;
    bounds.Y += GetSize().y + 4.0f;
    bounds.Width -= GetSize().x * 2.0f + 4.0f;
    bounds.Height -= GetSize().y * 2.0f + 4.0f;

	if(!bounds.Contains(_position) && _randomState == 0)
	{
		// Head towards centre
		auto temp = Vector2(viewport->GetWidth() / 2, viewport->GetHeight() / 2) - _position;
		_randomDirection = std::atan2f(temp.y, temp.x) + MathHelper::Random(0.0f, 1.0f) * M_PI - M_PI / 2.0f;
	}

	_randomState = (_randomState + 1) % 6;

	return true;
}