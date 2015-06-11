#pragma once

#include <windows.h>

namespace SGS2D 
{
	class BasicTimer
	{
	public:
		BasicTimer();
		void Reset();
		void Update(); 
		float GetTotal();
		float GetDelta();
	private:
		LARGE_INTEGER _frequency;
		LARGE_INTEGER _currentTime;
		LARGE_INTEGER _startTime;
		LARGE_INTEGER _lastTime;
		float _total;
		float _delta;
	};
}