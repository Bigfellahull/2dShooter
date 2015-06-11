#include "stdafx.h"
#include "Bullet.h"
#include "Art.h"
#include "MathHelper.h"
#include "ParticleManager.h"
#include "Viewport.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Bullet::Bullet(Vector2 position, Vector2 velocity) : Entity(Entity::Kind::Bullet)
{
	_texture = Art::GetInstance()->GetBullet();
	_position = position;
	_velocity = velocity;
	_orientation = static_cast<float>(std::atan2(_velocity.y, _velocity.x));
	_radius = 8;
}

void Bullet::Update(InputManager& inputManager, float totalTime, float deltaTime)
{
	if(_velocity.LengthSquared() > 0)
	{
		_orientation = static_cast<float>(std::atan2(_velocity.y, _velocity.x));
	}

	_position += _velocity;
	
	if (_position.x < 0 || _position.x > Viewport::GetInstance()->GetWidth() || _position.y < 0 || _position.y > Viewport::GetInstance()->GetHeight()) 
    {
        _isExpired = true;

		for(auto i = 0; i < 5; i++)
		{
            auto speed = 8.0f * (1.0f - 1 / MathHelper::Random(1.0f, 10.0f));
           
            auto velocity = MathHelper::NextVector2(speed, speed);

			ParticleManager::GetInstance()->CreateParticle(Art::GetInstance()->GetParticle(), _position, velocity, Vector4(0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f), 50.0f, Vector2(0.7, 0.7));
		}
    }
}