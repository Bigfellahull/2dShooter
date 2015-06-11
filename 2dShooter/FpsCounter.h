#pragma once

#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "SimpleMath.h"
#include <string>

namespace SGS2D
{
	class FpsCounter
	{
	public:
		FpsCounter();
		void Update(float delta);
		void Draw(DirectX::SpriteBatch& spriteBatch, DirectX::SpriteFont& spriteFont, DirectX::SimpleMath::Vector2 position);
	private:
		int _frameRate;
		int _frameCounter; 
		float _elapsedTime;
	};
}