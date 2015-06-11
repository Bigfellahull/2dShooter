#pragma once
#include "Texture2d.h"

namespace SGS2D
{
    class RenderTarget2d : public Texture2d
    {
    public:
        RenderTarget2d();

        void CreateRenderTarget(
            ID3D11Device* device,
            unsigned int width,
            unsigned int height
            );

        ID3D11RenderTargetView* const* GetRtv() const { return &_rtv; }
        ID3D11DepthStencilView* GetDsv() const { return _dsv; }
    protected:
        ID3D11RenderTargetView* _rtv;
        ID3D11DepthStencilView* _dsv;
    };
}