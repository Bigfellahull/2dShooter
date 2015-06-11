#include "stdafx.h"
#include "MathHelper.h"
#include <cstdlib>
#include <ctime>

using namespace SGS2D;
using namespace DirectX::SimpleMath;

bool MathHelper::_randomInit = false;

float MathHelper::Random(float min, float max)
{
    if (!MathHelper::_randomInit)
    {
        std::srand(static_cast<unsigned>(std::time(0)));
        MathHelper::_randomInit = true;
    }

	return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

int MathHelper::Random(int min, int max)
{
    if (!MathHelper::_randomInit)
    {
        std::srand(static_cast<unsigned>(std::time(0)));
        MathHelper::_randomInit = true;
    }

	return min + (std::rand() % (max - min + 1));
}

float MathHelper::Lerp(float value1, float value2, float amount)
{
	return value1 + (value2 - value1) * amount;
}

Vector2 MathHelper::NextVector2(float min, float max)
{
    auto theta = MathHelper::Random(0.0f, 1.0f) * 2.0f * M_PI;
    auto length = MathHelper::Random(min, max);
    return Vector2(length * static_cast<float>(std::cosf(theta)), length * static_cast<float>(std::sinf(theta)));
}