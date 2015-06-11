#pragma once

#include <d3d11.h>
#include <memory>

namespace SGS2D
{
	class Texture2d
	{
    public:
        Texture2d();
        
        void Load(
            ID3D11Device* device,
            const wchar_t* filename
            );

        ID3D11Texture2D* GetTexture2d() const { return _texture; }
        ID3D11ShaderResourceView* GetSrv() const { return _srv; }
        const D3D11_TEXTURE2D_DESC* GetDescription() const { return &_description; }
        
        float GetWidth() const { return _width; }
        float GetHeight() const { return _height; }
    protected:
        ID3D11Texture2D* _texture;
        ID3D11ShaderResourceView* _srv;
        D3D11_TEXTURE2D_DESC _description;
        float _width;
        float _height;
	};
}