#include "stdafx.h"
#include "ColourUtility.h"

using namespace SGS2D;
using namespace DirectX::SimpleMath;

Vector4 ColourUtility::HsvToColour(float h, float s, float v)
{
    if(h == 0 && s == 0)
	{
	    return Vector4(v, v, v, 1.0f);
	}
	
	float c = s * v;
	float x = c * (1 - std::abs(static_cast<int>(h) % 2 - 1));
	float m = v - c;
	
	if(h < 1)
	{
	    return Vector4(c + m, x + m, m, 1.0f);
	}
	else if (h < 2)
	{
	    return Vector4(x + m, c + m, m, 1.0f);
	}
	else if (h < 3)
	{
	    return Vector4(m, c + m, x + m, 1.0f);
	}
	else if (h < 4)
	{
	    return Vector4(m, x + m, c + m, 1.0f);
	}
	else if (h < 5)
	{
	    return Vector4(x + m, m, c + m, 1.0f);
	}
	
	return Vector4(c + m, m, x + m, 1.0f);
}
