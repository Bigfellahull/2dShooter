#pragma once

#include <windef.h>
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "Texture2d.h"

namespace SGS2D 
{
	struct Rectangle
	{
		Rectangle() {}
		Rectangle(int x, int y, int width, int height) : X(x), Y(y), Width(width), Height(height) {}
		int X;
		int Y;
		int Width;
		int Height;
		int Left() 
		{ 
			return X;
		}
		int Right() 
		{ 
			return X + Width;
		}
		int Top() 
		{ 
			return Y;
		}
		int Bottom() 
		{ 
			return Y + Height; 
		}
		RECT ToRect() 
		{
			RECT rect;
			rect.left = Left();
			rect.right = Right();
			rect.bottom = Bottom();
			rect.top = Top();

			return rect;
		}
		bool Contains(DirectX::SimpleMath::Vector2 point)
		{
			return point.x >= X &&
				point.y >= Y &&
				point.x < (X + Width) &&
				point.y < (Y + Height);
		}
        void Draw(DirectX::SpriteBatch& spriteBatch, Texture2d& texture)
        {
            spriteBatch.Draw(texture.GetSrv(), Rectangle(X, Y, Width, 1).ToRect(), nullptr, DirectX::Colors::White);
            spriteBatch.Draw(texture.GetSrv(), Rectangle(X, Y, 1, Height).ToRect(), nullptr, DirectX::Colors::White);
            spriteBatch.Draw(texture.GetSrv(), Rectangle(X + Width, Y, 1, Height).ToRect(), nullptr, DirectX::Colors::White);
            spriteBatch.Draw(texture.GetSrv(), Rectangle(X, Y + Height, Width, 1).ToRect(), nullptr, DirectX::Colors::White);
        }
	};
}