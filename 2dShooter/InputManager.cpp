#include "stdafx.h"
#include "InputManager.h"

using namespace SGS2D;
using namespace DirectX;

InputManager::InputManager()
{
	_gamePad.reset(new GamePad());
	_tracker.reset(new GamePad::ButtonStateTracker());
}

void InputManager::Update()
{
	auto state = _gamePad->GetState(0);

	if(state.IsConnected())
	{
		_tracker->Update(state);
	}
}

GamePad::State InputManager::GetState()
{
	return _gamePad->GetState(0);
}

GamePad::ButtonStateTracker& InputManager::GetTracker()
{
	return *_tracker;
}