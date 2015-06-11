#pragma once

#include "Singleton.h"

namespace SGS2D
{
	class PlayerStatus : public Singleton<PlayerStatus>
	{
	public:
		PlayerStatus();
		void Update(float totalTime, float deltaTime);
        void Reset();
        void AddPoints(int basePoints);
		void IncreaseMultiplier();
        void ResetMultiplier();
		void RemoveLife();
		bool IsGameOver();

		int GetLives();
		int GetMultiplier();
		int GetScore();
	private:
		int _lives;
		int _score;
		int _multiplier;
		float _maxMultiplier;
		float _multiplerExpiryTime;
		float _multiplierTimeLeft;
		int _scoreForExtraLife;
	};
}