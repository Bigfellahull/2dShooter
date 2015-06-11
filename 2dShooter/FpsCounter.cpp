#include "stdafx.h"
#include "FpsCounter.h"

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

FpsCounter::FpsCounter() : _frameRate(0), _frameCounter(0), _elapsedTime(0.0f)
{
}

void FpsCounter::Update(float delta)
{
	_elapsedTime += delta;
	if(_elapsedTime > 1)
	{
		_elapsedTime -= 1;
		_frameRate = _frameCounter;
		_frameCounter = 0;
	}
}

void FpsCounter::Draw(SpriteBatch& spriteBatch, SpriteFont& spriteFont, Vector2 position)
{
	_frameCounter++;
	spriteFont.DrawString(&spriteBatch, (L"FPS: " + std::to_wstring(_frameRate)).c_str(), position, Colors::White, 0.0, Vector2(0, 0), 0.5);
}