#pragma once

#include "Singleton.h"
#include "GamePad.h"
#include <memory>

namespace SGS2D
{
	class InputManager
	{
	public:
		InputManager();
		void Update();
		DirectX::GamePad::State GetState();
		DirectX::GamePad::ButtonStateTracker& GetTracker();
	private:
		std::unique_ptr<DirectX::GamePad> _gamePad;
		std::unique_ptr<DirectX::GamePad::ButtonStateTracker> _tracker;
	};
}