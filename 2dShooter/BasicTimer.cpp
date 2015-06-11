#include "stdafx.h"
#include "BasicTimer.h"
#include <exception>

using namespace SGS2D;

BasicTimer::BasicTimer()
{
	if(!QueryPerformanceFrequency(&_frequency)) 
	{
		throw new std::exception("No frequency");
	}

	Reset();
}

void BasicTimer::Reset()
{
	Update();

	_startTime = _currentTime;
	_total = 0.0f;
	_delta = 1.0f / 60.f;
}

void BasicTimer::Update()
{
	if(!QueryPerformanceCounter(&_currentTime)) 
	{
		throw new std::exception("No current time");
	}

	_total = static_cast<float>(static_cast<double>(_currentTime.QuadPart - _startTime.QuadPart) / static_cast<double>(_frequency.QuadPart));
		
	if (_lastTime.QuadPart == _startTime.QuadPart)
	{
		// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
		_delta = 1.0f / 60.0f;
	}
	else
	{
		_delta = static_cast<float>(static_cast<double>(_currentTime.QuadPart - _lastTime.QuadPart) / static_cast<double>(_frequency.QuadPart));
	}
		
	_lastTime = _currentTime;
}

float BasicTimer::GetDelta()
{
	return _delta;
}

float BasicTimer::GetTotal()
{
	return _total;
}