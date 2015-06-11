#pragma once

#include "SimpleMath.h"

namespace SGS2D
{
    class ColourUtility
	{
	public:
	    static DirectX::SimpleMath::Vector4 HsvToColour(float h, float s, float v);
	};
}