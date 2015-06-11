#include "stdafx.h"
#include "PlayerStatus.h"
#include "PlayerShip.h"

using namespace SGS2D;

PlayerStatus::PlayerStatus() : _multiplerExpiryTime(0.5f), _maxMultiplier(20)
{
	Reset();
}

void PlayerStatus::Reset()
{
	_score = 0;
	_multiplier = 1;
	_lives = 4;
	_scoreForExtraLife = 2000;
	_multiplierTimeLeft = 0;
}

void PlayerStatus::Update(float totalTime, float deltaTime)
{
	if(_multiplier > 1)
	{
		if((_multiplierTimeLeft -= deltaTime) <= 0)
		{
			_multiplierTimeLeft = _multiplerExpiryTime;
			ResetMultiplier();
		}
	}
}

void PlayerStatus::AddPoints(int basePoints)
{
	if(PlayerShip::GetInstance()->IsDead())
	{
		return;
	}

	_score += basePoints * _multiplier;

	while(_score >= _scoreForExtraLife)
	{
		_scoreForExtraLife += 2000;
		_lives++;
	}
}

void PlayerStatus::IncreaseMultiplier()
{
	if(PlayerShip::GetInstance()->IsDead())
	{
		return;
	}

	_multiplierTimeLeft = _multiplerExpiryTime;

	if(_multiplier < _maxMultiplier)
	{
		_multiplier++;
	}
}

void PlayerStatus::ResetMultiplier()
{
	_multiplier = 1;
}

void PlayerStatus::RemoveLife()
{
	_lives--;
}

int PlayerStatus::GetLives()
{
	return _lives;
}

int PlayerStatus::GetMultiplier()
{
	return _multiplier;
}

int PlayerStatus::GetScore()
{
	return _score;
}

bool PlayerStatus::IsGameOver()
{
	return _lives == 0;
}