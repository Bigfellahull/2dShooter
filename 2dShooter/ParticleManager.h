#pragma once

#include "CircularArray.h"
#include "SpriteBatch.h"
#include "SimpleMath.h"
#include "Texture2d.h"
#include "Singleton.h"

namespace SGS2D
{
	class ParticleManager : public Singleton<ParticleManager>
	{
	public:
        enum ParticleType
        {
            None = 0,
            Enemy,
            Bullet,
            IgnoreGravity
        };

		struct Particle
		{
		public:
			Texture2d* Texture;
			DirectX::SimpleMath::Vector2 Position;
			DirectX::SimpleMath::Vector2 Velocity;
			DirectX::SimpleMath::Vector2 Scale;
			DirectX::SimpleMath::Vector4 Colour;
			float Orientation;
			float Duration;
			float PercentLife;
            ParticleType Type;

            Particle() :
                Texture(),
                Position(DirectX::SimpleMath::Vector2(0, 0)),
                Velocity(DirectX::SimpleMath::Vector2(0, 0)),
                Colour(DirectX::SimpleMath::Vector4(1, 1, 1, 1)),
                Scale(DirectX::SimpleMath::Vector2(1, 1)),
                Orientation(0.0f),
                Duration(100.0f),
                PercentLife(1.0f),
                Type(ParticleType::None)
			{};
		};

		ParticleManager();
		void Update();
		void CreateParticle(Texture2d* texture, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 velocity, DirectX::SimpleMath::Vector4 colour, float duration, DirectX::SimpleMath::Vector2 scale, ParticleType type = ParticleType::None, float theta = 0);
        void Update(float totalTime, float deltaTime);
        void Draw(DirectX::SpriteBatch& spriteBatch, float totalTime, float deltaTime);
        int ParticleCount();
    private:
        CircularArray<Particle> _particles;
        void Swap(int indexA, int indexB);
	};
}