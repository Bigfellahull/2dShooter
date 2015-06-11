#pragma once

#include "Singleton.h"
#include <d3d11.h>
#include "Rectangle.h"

namespace SGS2D 
{
	class Viewport : public Singleton<Viewport>
	{
	public:
		Viewport() {}
		void SetViewport(D3D11_VIEWPORT viewport)
		{
			_viewport = viewport;
		}
		int GetX()
		{
			return _viewport.TopLeftX;
		}
		int GetY()
		{
			return _viewport.TopLeftY;
		}
		int GetWidth() const
		{
			return _viewport.Width;
		}
		int GetHeight() const
		{
			return _viewport.Height;
		}
		Rectangle GetBounds() const
		{
			return Rectangle(0, 0, _viewport.Width, _viewport.Height);
		}
		Rectangle GetTitleSafeArea()
		{
			auto bounds = GetBounds();

			auto xDelta = 0.80f * _viewport.Width;
			auto yDelta = 0.80f * _viewport.Height;

			bounds.X += xDelta;
			bounds.Y += yDelta;

			bounds.Width -= xDelta;
			bounds.Height -= yDelta;

			return bounds;
		}
	private:
		D3D11_VIEWPORT _viewport;
	};
}