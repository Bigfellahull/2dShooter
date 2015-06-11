#include "stdafx.h"
#include "Entity.h"
#include "DDSTextureLoader.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Entity::Entity(Kind kind) : _kind(kind)
{
	_colour = Vector4(1, 1, 1, 1);
	_position = Vector2::Zero;
	_velocity = Vector2::Zero;
	_orientation = 0.0f;
	_radius = 0.0f;
	_isExpired = false;
}

Vector4 Entity::GetColour() 
{
	return _colour;
}

void Entity::SetColour(Vector4 colour)
{
	_colour = colour;
}

Vector2 Entity::GetPosition()
{
	return _position;
}

void Entity::SetPosition(Vector2 position)
{
	_position = position;
}

Vector2 Entity::GetVelocity()
{
	return _velocity;
}

void Entity::SetVelocity(Vector2 velocity)
{
	_velocity = velocity;
}

float Entity::GetOrientation()
{
	return _orientation;
}

void Entity::SetOrientation(float orientation)
{
	_orientation = orientation;
}

float Entity::GetRadius()
{
	return _radius;
}

void Entity::SetRadius(float radius)
{
	_radius = radius;
}

float Entity::GetIsExpired()
{
	return _isExpired;
}

void Entity::SetIsExpired(bool expired)
{
	_isExpired = expired;
}

Vector2 Entity::GetSize()
{
    return Vector2(_texture->GetWidth(), _texture->GetHeight());
}

Entity::Kind Entity::GetKind()
{
    return _kind;
}

void Entity::Draw(SpriteBatch& spriteBatch, float totalTime, float deltaTime)
{
	auto size = GetSize();
	spriteBatch.Draw(_texture->GetSrv(), _position, nullptr, _colour, _orientation, Vector2(size.x / 2, size.y / 2), 1.0f, SpriteEffects::SpriteEffects_None, 0);
}