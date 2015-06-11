#include "stdafx.h"
#include "BlackHole.h"
#include "Art.h"
#include "Enemy.h"
#include "MathHelper.h"
#include "PlayerStatus.h"
#include "ColourUtility.h"
#include "ParticleManager.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

BlackHole::BlackHole(Vector2 position) : 
    _pointValue(10),
    _sprayAngle(0),
    Entity(Entity::Kind::BlackHole)
{
	_texture = Art::GetInstance()->GetBlackHole();
	_position = position;
	_radius = _texture->GetWidth() / 2.0f;
	_hitpoints = 20;
}

void BlackHole::WasShot(float totalTime, float deltaTime)
{
	_hitpoints--;

    auto hue = std::fmodf(3.0f * totalTime, 6);
    auto colour = ColourUtility::HsvToColour(hue, 0.5f, 1.0f);
    auto numberOfParticles = 200;
        
    for (auto i = 0; i < numberOfParticles; i++)
    {
        auto speed = 16.0f * (1.0f - 1 / MathHelper::Random(1.0f, 10.0f));
                    
        auto velocity = MathHelper::NextVector2(speed, speed);
        
        auto position = _position + MathHelper::Random(2.0f, 8.0f) * velocity;

        ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), position, velocity, colour, 140.0f, Vector2(0.7f, 0.7f), ParticleManager::ParticleType::IgnoreGravity);
    }
	
	if(_hitpoints <= 0)
	{
		_isExpired = true;

        PlayerStatus::GetInstance()->AddPoints(_pointValue);
        PlayerStatus::GetInstance()->IncreaseMultiplier();
	}
}

void BlackHole::Kill(float totalTime, float deltaTime)
{
	_hitpoints = 0;
	WasShot(totalTime, deltaTime);
}

void BlackHole::Update(InputManager& inputManager, float totalTime, float deltaTime)
{
    auto entities = EntityManager::GetInstance()->GetNearbyEntities(_position, 200);

    for (auto i = entities.begin(); i != entities.end(); ++i)
    {
        if ((*i) == nullptr || (*i) == this || (*i)->GetKind() == Entity::Kind::Player)
        {
            continue;
        }

        if ((*i)->GetKind() == Entity::Kind::Enemy)
        {
            if (!static_cast<SGS2D::Enemy*>(*i)->GetIsActive())
            {
                continue;
            }
        }

        if ((*i)->GetKind() == Entity::Kind::Bullet)
        {
            auto temp = ((*i)->GetPosition() - _position);
            temp.Normalize();
            (*i)->SetVelocity((*i)->GetVelocity() + temp * 0.5f);
        }
        else
        {
            auto temp = _position - (*i)->GetPosition();
            temp.Normalize();

            (*i)->SetVelocity((*i)->GetVelocity() + temp * MathHelper::Lerp(1.0f, 0.0f, temp.Length() / 250.0f));
        }
    }

    if (((int)(totalTime * 1000) / 250) % 2 == 0)
    {
        auto sprayVelocity = MathHelper::Random(12.0f, 15.0f) * Vector2(static_cast<float>(std::cosf(_sprayAngle)), static_cast<float>(std::sinf(_sprayAngle)));
        auto colour = ColourUtility::HsvToColour(5.0f, 0.5f, 0.8f);
               
        auto velocity = MathHelper::NextVector2(3.0f, 7.0f);
        
        auto position = _position + 2.0f * Vector2(sprayVelocity.y, -sprayVelocity.x) + velocity;
        
        ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), position, velocity, colour, 190.0f, Vector2(0.7f, 0.7f));
    }

    _sprayAngle -= M_PI * 2.0f / 50.0f;
}

void BlackHole::Draw(SpriteBatch& spriteBatch, float totalTime, float deltaTime)
{
	auto scale = 1.0f + 0.1f * std::sin(totalTime * 10.0f);
	auto size = GetSize();
	spriteBatch.Draw(_texture->GetSrv(), _position, nullptr, _colour, _orientation, Vector2(size.x / 2, size.y / 2), scale, SpriteEffects::SpriteEffects_None, 0);
}