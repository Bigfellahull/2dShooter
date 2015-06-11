#include "stdafx.h"
#include "PlayerShip.h"
#include "Art.h"
#include "Bullet.h"
#include "MathHelper.h"
#include "InputManager.h"
#include "PlayerStatus.h"
#include "Viewport.h"
#include "ParticleManager.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerShip::PlayerShip() : Entity(Entity::Kind::Player)
{
	_texture = Art::GetInstance()->GetPlayer();
	_position = Vector2(200, 200);
	_radius = 10.0f;
	_cooldownFrames = 5;
	_cooldownRemaining = 0;
    _framesUntilRespawn = 0;
}

bool PlayerShip::IsDead()
{
    return _framesUntilRespawn > 0;
}

void PlayerShip::Update(InputManager& inputManager, float totalTime, float deltaTime)
{
	auto viewport = Viewport::GetInstance();
	auto entityManager = EntityManager::GetInstance();

	auto state = inputManager.GetState();
	auto tracker = inputManager.GetTracker();

    if (IsDead())
    {
        _framesUntilRespawn--;

		if(_framesUntilRespawn == 0 && PlayerStatus::GetInstance()->IsGameOver())
		{
			// Reset score on spawn from gameover
			PlayerStatus::GetInstance()->Reset();
		}

		return;
    }

    // Movement
	auto direction = Vector2(state.thumbSticks.leftX, state.thumbSticks.leftY);
	direction.y *= -1;

	if(direction.LengthSquared() > 1)
	{
		direction.Normalize();
	}

	_velocity = 10.0f * direction;
	_position += _velocity;

	_position.Clamp(GetSize(), (Vector2(viewport->GetWidth(), viewport->GetHeight()) - GetSize()));
    
	if(_velocity.LengthSquared() > 0)
	{
		_orientation = static_cast<float>(std::atan2(_velocity.y, _velocity.x));
	}
    
    // Bullets!
	auto aimDirection = Vector2(state.thumbSticks.rightX, state.thumbSticks.rightY);
	aimDirection.y *= -1;
	
    if (aimDirection.LengthSquared() > 1) 
    {
        aimDirection.Normalize();
    }
	
	if(aimDirection.LengthSquared() > 0 && _cooldownRemaining <= 0)
	{
		_cooldownRemaining = _cooldownFrames;

		auto aimAngle = static_cast<float>(std::atan2(aimDirection.y, aimDirection.x));
        auto aimQuat = Quaternion::CreateFromYawPitchRoll(0, 0, aimAngle);
        
        auto randomSpread = (MathHelper::Random(0.0f, 1.0f) * 0.02) + (MathHelper::Random(0.0f, 1.0f) * 0.02);
        auto vel = 16.0f * Vector2(static_cast<float>(std::cos(aimAngle + randomSpread)), static_cast<float>(std::sin(aimAngle + randomSpread)));
		
		auto offset = Vector2::Transform(Vector2(40, -8), aimQuat);
        entityManager->Add(new SGS2D::Bullet(_position + offset, vel));

		offset = Vector2::Transform(Vector2(40, 8), aimQuat);
        entityManager->Add(new SGS2D::Bullet(_position + offset, vel));
	}
    
	if(_cooldownRemaining > 0)
	{
		_cooldownRemaining--;
	}

    MakeExhaustFire(totalTime, deltaTime);
}

void PlayerShip::MakeExhaustFire(float totalTime, float deltaTime)
{
    if (_velocity.LengthSquared() < 0.1f)
    {
        return;
    }

    auto orientation = std::atan2(_velocity.y, _velocity.x);
    auto rotation = Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, orientation);

    auto baseVelocity = _velocity * (-3.0f / _velocity.Length());
    auto perpendicularVelocity = Vector2(baseVelocity.y, -baseVelocity.x) * (0.6f * static_cast<float>(std::sin(totalTime * 10.0f)));

    auto white = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    auto color1 = Vector4(0.78f, 0.15f, 0.04f, 1.0f);
    auto color2 = Vector4(1.0f, 0.73f, 0.12f, 1.0f);
    auto position = _position + Vector2::Transform(Vector2(-25, 0), rotation);

    auto alpha = 0.7f;

    auto velocityMid = baseVelocity + MathHelper::NextVector2(0.0f, 1.0f);

    ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), position, velocityMid, white * alpha, 40.0f, Vector2(0.4, 0.7));
    ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetGlowParticle(), position, velocityMid, color2 * alpha, 40.0f, Vector2(0.4, 0.7));
        
    auto velocitySide1 = baseVelocity + perpendicularVelocity + MathHelper::NextVector2(0.0f, 0.3f);
    auto velocitySide2 = baseVelocity - perpendicularVelocity + MathHelper::NextVector2(0.0f, 0.3f);

    ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), position, velocitySide1, white * alpha, 50.0f, Vector2(0.4, 0.7));
    ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetGlowParticle(), position, velocitySide1, color1 * alpha, 50.0f, Vector2(0.4, 0.7));

    ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), position, velocitySide2, white * alpha, 50.0f, Vector2(0.4, 0.7));
    ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetGlowParticle(), position, velocitySide2, color1 * alpha, 50.0f, Vector2(0.4, 0.7));
}

void PlayerShip::Draw(SpriteBatch& spriteBatch, float totalTime, float deltaTime)
{
    if (!IsDead())
    {
        Entity::Draw(spriteBatch, totalTime, deltaTime); 
    }
}

void PlayerShip::Kill()
{
	PlayerStatus::GetInstance()->RemoveLife();
	_framesUntilRespawn = PlayerStatus::GetInstance()->IsGameOver() ? 300 : 120;

	auto colour1 = Vector4(1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f);
	auto colour2 = Vector4(0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f);

	for(int i = 0; i < 400; i++)
	{
		auto speed = 22.0f * (1.0f - 1 / MathHelper::Random(1.0f, 10.0f));

        auto velocity = MathHelper::NextVector2(speed, speed);
                
		ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), _position, velocity, Vector4::Lerp(colour1, colour2, MathHelper::Random(0.0f, 1.0f)), 190.0f, Vector2(0.7, 0.7));
	}
}