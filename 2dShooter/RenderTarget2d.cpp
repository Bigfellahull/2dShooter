#include "stdafx.h"
#include "RenderTarget2d.h"

using namespace SGS2D;

RenderTarget2d::RenderTarget2d() :
    Texture2d(),
    _rtv(),
    _dsv()
{
}

void RenderTarget2d::CreateRenderTarget(
    ID3D11Device* device,
    unsigned int width,
    unsigned int height
    )
{
    // Populate the texture description struct with the appropriate values.
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    // Store the texture description so we can easily access it later.
    _description = texDesc;
    _width = static_cast<float>(texDesc.Width);
    _height = static_cast<float>(texDesc.Height);

    // Create the ID3D11Texture2D resource.
    DX::ThrowIfFailed(
        device->CreateTexture2D(&texDesc, nullptr, &_texture)
        );

    // Populate the render target view description with the appropriate values.
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    // Create an ID3D11RenderTargetView of the texture resource so that we can bind it for use as a render target.
    DX::ThrowIfFailed(
        device->CreateRenderTargetView(_texture, &rtvDesc, &_rtv)
        );

    // To draw the render target to another render target (including the back buffer), we need a shader resource view of it.
    // Populate the shader resource view description with the appropriate values.
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    // Create an ID3D11ShaderResourceView of the texture resource so that we can bind it for use in shaders.
    DX::ThrowIfFailed(
        device->CreateShaderResourceView(_texture, &srvDesc, &_srv)
        );

    // Create depth stencil buffer
    ID3D11Texture2D* depthStencilTexture;

    // It will have almost entirely the same values as the render target so we only change the
    // texture description values that are different for the depth stencil buffer.
    texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    // Create the ID3D11Texture2D for the depth stencil buffer.
    DX::ThrowIfFailed(
        device->CreateTexture2D(&texDesc, nullptr, &depthStencilTexture)
        );

    // Populate a depth stencil view description.
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    // Create the ID3D11DepthStencilView of the depth stencil texture resource so that we can bind it for use as a depth stencil buffer.
    DX::ThrowIfFailed(
        device->CreateDepthStencilView(depthStencilTexture, &dsvDesc, &_dsv)
        );
}