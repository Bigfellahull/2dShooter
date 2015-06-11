#include "stdafx.h"
#include "ParticleManager.h"
#include <algorithm>
#include "Viewport.h"
#include "EntityManager.h"
#include "BlackHole.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

ParticleManager::ParticleManager() : _particles(2000)
{
}

void ParticleManager::CreateParticle(Texture2d* texture, Vector2 position, Vector2 velocity, Vector4 colour, float duration, Vector2 scale, ParticleType type, float theta)
{
    int index;

    if (_particles.GetCount() == _particles.GetCapacity())
    {
        index = 0;
        _particles.SetStart(_particles.GetStart() + 1);
    }
    else
    {
        index = _particles.GetCount();
        _particles.SetCount(_particles.GetCount() + 1);
    }

    Particle& particle = _particles[index];

    particle.Texture = texture;
    particle.Position = position;
	particle.Velocity = velocity;
    particle.Colour = colour;
	particle.Duration = duration;
    particle.PercentLife = 1.0f;
    particle.Scale = scale;
    particle.Type = type;
    particle.Orientation = theta;
}

void ParticleManager::Update(float totalTime, float deltaTime)
{
    auto removalCount = 0;

    auto blackHoles = EntityManager::GetInstance()->GetBlackHoles();

    for (auto i = 0; i < _particles.GetCount(); i++)
    {
        Particle& particle = _particles[i];
        
		auto x = particle.Position;
		particle.Position += particle.Velocity;
		particle.Orientation = static_cast<float>(std::atan2(particle.Velocity.y, particle.Velocity.x));

		auto speed = particle.Velocity.Length();
		auto alpha = std::min(1.0f, std::min(particle.PercentLife * 2, speed * 1.0f));
		alpha *= alpha;

		// Fade and shorten particle if near end of life or moving slowly
		particle.Colour.w = alpha;
		particle.Scale.x = 1.0f * std::min(std::min(1.0f, 0.2f * speed + 0.1f), alpha);

		if(particle.Position.x < 0)
		{
			particle.Velocity.x = std::abs(particle.Velocity.x);
		}
		else if(particle.Position.x > Viewport::GetInstance()->GetWidth())
		{
			particle.Velocity.x = -std::abs(particle.Velocity.x);
		}
		
		if(particle.Position.y < 0)
		{
			particle.Velocity.y = std::abs(particle.Velocity.y);
		}
		else if(particle.Position.y > Viewport::GetInstance()->GetHeight())
		{
			particle.Velocity.y = -std::abs(particle.Velocity.y);
		}

        if (particle.Type != ParticleType::IgnoreGravity)
        {
            for (auto i = blackHoles.begin(); i != blackHoles.end(); ++i)
            {
                auto dPos = (*i)->GetPosition() - particle.Position;
                auto distance = dPos.Length();
                Vector2 n = dPos / distance;
                particle.Velocity += 10000 * n / (distance * distance + 10000);
                
                if (distance < 400)
                {
                    particle.Velocity += 45 * Vector2(n.y, -n.x) / (distance + 100);
                }
            }
        }

		particle.Velocity *= 0.97f;

        particle.PercentLife -= 1.0f / particle.Duration;
		
        Swap(i - removalCount, i);

        if (particle.PercentLife < 0)
        {
            removalCount++;
        }
    }

    _particles.SetCount(_particles.GetCount() - removalCount);
}

void ParticleManager::Swap(int indexA, int indexB)
{
    auto temp = _particles[indexA];
    _particles[indexA] = _particles[indexB];
    _particles[indexB] = temp;
}

void ParticleManager::Draw(SpriteBatch& spriteBatch, float totalTime, float deltaTime)
{
    for (auto i = 0; i < _particles.GetCount(); i++)
    {
        auto particle = _particles[i];

        auto origin = Vector2(particle.Texture->GetWidth() / 2, particle.Texture->GetHeight() / 2);
        
        spriteBatch.Draw(particle.Texture->GetSrv(), particle.Position, nullptr, particle.Colour, particle.Orientation, origin, particle.Scale, SpriteEffects::SpriteEffects_None, 0);
    }
}

int ParticleManager::ParticleCount()
{
    return _particles.GetCount();
}