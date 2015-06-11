#pragma once

#include "SimpleMath.h"

namespace SGS2D
{
    class MathHelper
    {
    public:
		static float Random(float min, float max);
		static int Random(int min, int max);
		static float Lerp(float value1, float value2, float amount);
        static DirectX::SimpleMath::Vector2 NextVector2(float min, float max);
    private:
        static bool _randomInit;
    };
}