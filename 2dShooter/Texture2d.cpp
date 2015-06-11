#include "stdafx.h"
#include "Texture2d.h"
#include "DDSTextureLoader.h"

using namespace SGS2D;
using namespace DirectX;

Texture2d::Texture2d() :
    _texture(),
    _srv(),
    _description(),
    _width(),
    _height()
{
}

void Texture2d::Load(
    ID3D11Device* device,
    const wchar_t* filename
    )
{
    ID3D11Resource* resource;
    
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, filename, &resource, &_srv)
        );

    _texture = static_cast<ID3D11Texture2D*>(resource);

    _texture->GetDesc(&_description);
    _width = static_cast<float>(_description.Width);
    _height = static_cast<float>(_description.Height);
}