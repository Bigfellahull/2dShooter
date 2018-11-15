#include "stdafx.h"
#include "2dShooter.h"

#include "CommonStates.h"
#include "DirectXColors.h"
#include "SpriteFont.h"

#include "BasicTimer.h"
#include "FpsCounter.h"
#include "Art.h"
#include "EntityManager.h"
#include "InputManager.h"
#include "PlayerShip.h"
#include "EnemySpawner.h"
#include "PlayerStatus.h"
#include "ParticleManager.h"
#include "Viewport.h"
#include "Camera.h"
#include "RenderTarget2d.h"
#include "BloomCBuffers.h"
#include "ReadData.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "xinput.lib")

using namespace SGS2D;
using namespace DirectX;
using namespace DirectX::SimpleMath;

// Global Variables:
HWND hWnd = nullptr; // Current window

D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* d3dDevice = nullptr;
ID3D11DeviceContext* d3dDeviceContext = nullptr;
IDXGISwapChain* swapChain = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11Texture2D* depthStencil = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;
ID3D11ShaderResourceView* shaderResourceView = nullptr;

RenderTarget2d sceneRenderTarget;
RenderTarget2d renderTargetOne;
RenderTarget2d renderTargetTwo;

ID3D11Buffer* extractD3DBuffer;
ID3D11Buffer* blurD3DBuffer;
ID3D11Buffer* combineD3DBuffer;
ID3D11PixelShader* extractPixelShader;
ID3D11PixelShader* blurPixelShader;
ID3D11PixelShader* combinePixelShader;

BloomExtractCBuffer extractCBuffer;
BloomBlurCBuffer blurCBuffer;
BloomCombineCBuffer bloomCombineCBuffer;

bool bloomEnabled = true;

std::unique_ptr<InputManager> inputManager;

std::unique_ptr<SpriteBatch> spriteBatch;
std::unique_ptr<SpriteFont> spriteFont;
std::unique_ptr<FpsCounter> fpsCounter;

std::unique_ptr<Camera> camera;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if(FAILED(InitWindow(hInstance, nCmdShow))) 
	{
		return 0;
	}

	if(FAILED(InitDevice())) 
	{
		CleanupDevice();
		return 0;
	}

	LoadContent();

	BasicTimer timer;

	auto nextTick = GetTickCount();
	auto sleepTime = 0;

    MSG msg = {0};
    while(WM_QUIT != msg.message)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			timer.Update();

			auto total = timer.GetTotal();
			auto delta = timer.GetDelta();
			
			Update(total, delta);
			Render(total, delta);
			
			nextTick += (1000/60); // 60 frames per second
			sleepTime = nextTick - GetTickCount();

			if(sleepTime >= 0)
			{
				Sleep(sleepTime);
			}
			else
			{
				// Running behind - keep calling update until caught up?
			}
        }
    }

    CleanupDevice();

    return (int)msg.wParam;
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"SGS2DClass";
	wcex.hIconSm = nullptr;

	if(!RegisterClassEx(&wcex)) 
	{
		return E_FAIL;
	}
	
	RECT rc = { 0, 0, 1280, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	
	hWnd = CreateWindow(L"SGS2DClass", L"SGS2D - Shooter", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	
	if(!hWnd) 
	{
		return E_FAIL;
	}
	
	ShowWindow(hWnd, nCmdShow);

    return S_OK;
}

HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
	GetClientRect(hWnd, &rc);
	auto width = rc.right - rc.left;
    auto height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    auto numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    auto numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for(unsigned int driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &swapChain, &d3dDevice, &featureLevel, &d3dDeviceContext);
        if(SUCCEEDED(hr))
		{
            break;
		}
    }

    if(FAILED(hr)) 
	{
        return hr;
	}

    // Create a render target
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if(FAILED(hr))
	{
		return hr;
	}
    
    // Create shader resouce view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = d3dDevice->CreateShaderResourceView(backBuffer, &srvDesc, &shaderResourceView);
    if (FAILED(hr))
    {
        return hr;
    }

	hr = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	backBuffer->Release();
	if(FAILED(hr)) 
	{
		return hr;
	}
	
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

	hr = d3dDevice->CreateTexture2D(&descDepth, nullptr, &depthStencil);
	if(FAILED(hr)) 
	{
		return hr;
	}

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

	hr = d3dDevice->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);
	if(FAILED(hr)) 
	{
		return hr;
	}

	// Set render target
    d3dDeviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    d3dDeviceContext->RSSetViewports(1, &vp);

	// Set the viewport on our helper wrapper
	SGS2D::Viewport::GetInstance()->SetViewport(vp);

    return S_OK;
}

void CleanupDevice()
{    
	if(depthStencilView)
	{
		depthStencilView->Release();
	}
    
	if(depthStencil)
	{
		depthStencil->Release();
	}
    
	if(renderTargetView)
	{
		renderTargetView->Release();
	}
    
	if(swapChain)
	{
		swapChain->Release();
	}
	
	if(d3dDeviceContext)
	{
		d3dDeviceContext->ClearState();
		d3dDeviceContext->Release();
	}

    if(d3dDevice)
	{
		d3dDevice->Release();
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch(message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void LoadContent()
{
	Art::GetInstance()->Load(*d3dDevice);

    camera.reset(new Camera(SGS2D::Viewport::GetInstance()));
	
	fpsCounter.reset(new FpsCounter());
	spriteBatch.reset(new SpriteBatch(d3dDeviceContext));
	spriteFont.reset(new SpriteFont(d3dDevice, L"..\\Debug\\italic.spritefont"));

    inputManager.reset(new InputManager());

	EntityManager::GetInstance()->Add(PlayerShip::GetInstance());

    // Bloom
    sceneRenderTarget.CreateRenderTarget(d3dDevice, SGS2D::Viewport::GetInstance()->GetWidth(), SGS2D::Viewport::GetInstance()->GetHeight());
    renderTargetOne.CreateRenderTarget(d3dDevice, SGS2D::Viewport::GetInstance()->GetWidth() * 0.25f, SGS2D::Viewport::GetInstance()->GetHeight() * 0.25f);
    renderTargetTwo.CreateRenderTarget(d3dDevice, SGS2D::Viewport::GetInstance()->GetWidth() * 0.25f, SGS2D::Viewport::GetInstance()->GetHeight() * 0.25f);

    extractCBuffer = BloomExtractCBuffer(0.25f);
    blurCBuffer = BloomBlurCBuffer();
    bloomCombineCBuffer = BloomCombineCBuffer(1.5f, 1.5f, 1.0f, 1.0f);
    
    D3D11_BUFFER_DESC cbufferDesc = {};
    cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbufferDesc.Usage = D3D11_USAGE_DEFAULT;

    cbufferDesc.ByteWidth = (sizeof(BloomExtractCBuffer) + 15) / 16 * 16;

    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&cbufferDesc, nullptr, &extractD3DBuffer)
        );

    cbufferDesc.ByteWidth = (sizeof(BloomBlurCBuffer) + 15) / 16 * 16;
    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&cbufferDesc, nullptr, &blurD3DBuffer)
        );

    cbufferDesc.ByteWidth = (sizeof(BloomCombineCBuffer) + 15) / 16 * 16;
    DX::ThrowIfFailed(
        d3dDevice->CreateBuffer(&cbufferDesc, nullptr, &combineD3DBuffer)
        );

    // Shaders
    
    auto blob = DX::ReadData(L"BloomExtractPixelShader.cso");

    DX::ThrowIfFailed(d3dDevice->CreatePixelShader(&blob.front(), blob.size(), nullptr, &extractPixelShader));

    auto blob2 = DX::ReadData(L"BloomBlurPixelShader.cso");

    DX::ThrowIfFailed(d3dDevice->CreatePixelShader(&blob2.front(), blob2.size(), nullptr, &blurPixelShader));

    auto blob3 = DX::ReadData(L"BloomCombinePixelShader.cso");

    DX::ThrowIfFailed(d3dDevice->CreatePixelShader(&blob3.front(), blob3.size(), nullptr, &combinePixelShader));
}

void Update(float totalTime, float deltaTime)
{
	fpsCounter->Update(deltaTime);
    inputManager->Update();

	PlayerStatus::GetInstance()->Update(totalTime, deltaTime);
	EnemySpawner::GetInstance()->Update();
	EntityManager::GetInstance()->Update(*inputManager, totalTime, deltaTime);
    ParticleManager::GetInstance()->Update(totalTime, deltaTime);

    if (inputManager->GetTracker().a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
    {
        bloomEnabled = !bloomEnabled;
    }

    /*if (InputManager::GetInstance()->GetTracker().dpadDown == GamePad::ButtonStateTracker::ButtonState::HELD)
    {
        camera->Move(Vector2(0, -2));
    }
    else if (InputManager::GetInstance()->GetTracker().dpadUp == GamePad::ButtonStateTracker::ButtonState::HELD)
    {
        camera->Move(Vector2(0, 2));
    }
    else if (InputManager::GetInstance()->GetTracker().dpadLeft == GamePad::ButtonStateTracker::ButtonState::HELD)
    {
        camera->Move(Vector2(-2, 0));
    }
    else if (InputManager::GetInstance()->GetTracker().dpadRight == GamePad::ButtonStateTracker::ButtonState::HELD)
    {
        camera->Move(Vector2(2, 0));
    }

    if (InputManager::GetInstance()->GetTracker().a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
    {
        camera->Zoom(0.5f);
    }
    else if (InputManager::GetInstance()->GetTracker().b == GamePad::ButtonStateTracker::ButtonState::PRESSED)
    {
        camera->Zoom(-0.5f);
    }*/
}

void Render(float totalTime, float deltaTime)
{
	// Clear the back buffer
	d3dDeviceContext->ClearRenderTargetView(renderTargetView, Colors::Black);

    // Clear the depth buffer to 1.0 (max depth)
    d3dDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	CommonStates states(d3dDevice);

    spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Texture, states.Additive());

    spriteBatch->Draw(Art::GetInstance()->GetStarfield()->GetSrv(), SGS2D::Viewport::GetInstance()->GetBounds().ToRect(), Vector4(1.0f, 1.0f, 1.0f, 0.5f));

    spriteBatch->End();

	spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Texture, states.Additive(), nullptr, nullptr, nullptr, nullptr, camera->GetViewMatrix());    
    
	if(!PlayerStatus::GetInstance()->IsGameOver())
	{
        EntityManager::GetInstance()->Draw(*spriteBatch, totalTime, deltaTime);
	}

    ParticleManager::GetInstance()->Draw(*spriteBatch, totalTime, deltaTime);

    spriteBatch->End();


    // BLOOM TEST

    if (bloomEnabled)
    {

        ID3D11ShaderResourceView* const nullSRV[] = { nullptr };

        // Duplicate the contents of renderTargetSRV to a holding texture.
        d3dDeviceContext->OMSetRenderTargets(1, sceneRenderTarget.GetRtv(), nullptr);


        // We use an opaque blend state and tell SpriteBatch to disregard depth so that we can avoid clearing the rendertarget and depth stencil buffer. This saves us
        // quite a bit of rendering time when working with GPUs that have a low fillrate. Even if there are transparent areas of the existing scene, using opaque will
        // just copy those transparent values over such that they'd still be transparent for anything that followed bloom.
        spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred, states.Opaque(), nullptr, states.DepthNone());
        spriteBatch->Draw(shaderResourceView, DirectX::XMFLOAT2(0.0f, 0.0f), nullptr);
        spriteBatch->End();

        // Brightness
        {
            d3dDeviceContext->UpdateSubresource(extractD3DBuffer, 0, nullptr, &extractCBuffer, 0, 0);

            d3dDeviceContext->OMSetRenderTargets(1, renderTargetOne.GetRtv(), nullptr);

            auto context = d3dDeviceContext;
            auto pixelShader = extractPixelShader;
            auto buffer = extractD3DBuffer;

            spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred, states.Opaque(), nullptr, states.DepthNone(), nullptr, [context, pixelShader, buffer]()
            {
                d3dDeviceContext->PSSetShader(extractPixelShader, nullptr, 0);
                d3dDeviceContext->PSSetConstantBuffers(0, 1, &buffer);
            });

            RECT rect = { 0L, 0L, static_cast<long>(renderTargetOne.GetWidth()), static_cast<long>(renderTargetOne.GetHeight()) };
            spriteBatch->Draw(sceneRenderTarget.GetSrv(), rect, nullptr);
            spriteBatch->End();
        }

        // Blur Horizontally
    {
        float gaussian;
        float offset = 0.0f;
        float deltaX;
        float deltaY;

        deltaX = 1.0f / renderTargetOne.GetWidth();
        deltaY = 0.0f;

        // sigma is the blur amount.
        auto sigmaSquared = 4.0f * 4.0f;

        // Calculate the one dimensional gaussian blur weight value for the given offset. (See: http://en.wikipedia.org/wiki/Gaussian_blur ).
        gaussian = static_cast<float>((1.0 / sqrtf(2.0f * XM_PI * sigmaSquared)) *
            expf(-(offset * offset) / (2 * sigmaSquared)));

        blurCBuffer.SampleOffsetsAndWeights[0].x = 0.0f;
        blurCBuffer.SampleOffsetsAndWeights[0].y = 0.0f;
        blurCBuffer.SampleOffsetsAndWeights[0].z = gaussian;

        // The number of samples used in blurring each pixel.
        int sampleCount = BLUR_SAMPLE_COUNT;

        // Running total of weights.
        float totalWeights = gaussian;

        // Add pairs of additional sample taps, positioned
        // along a line in both directions from the center.
        for (int i = 0; i < sampleCount / 2; i++)
        {
            // Store weights for the positive and negative taps.
            offset = static_cast<float>(i + 1);

            // Calculate the one dimensional gaussian blur weight value for the given offset.
            gaussian = static_cast<float>((1.0 / sqrtf(2.0f * XM_PI * sigmaSquared)) *
                expf(-(offset * offset) / (2 * sigmaSquared)));

            float weight = gaussian;

            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 1].z = weight;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 2].z = weight;

            totalWeights += weight * 2;

            // To get the maximum amount of blurring from a limited number of
            // pixel shader samples, we take advantage of the bilinear filtering
            // hardware inside the texture fetch unit. If we position our texture
            // coordinates exactly halfway between two texels, the filtering unit
            // will average them for us, giving two samples for the price of one.
            // This allows us to step in units of two texels per sample, rather
            // than just one at a time. The 1.5 offset kicks things off by
            // positioning us nicely in between two texels.
            float sampleOffset = i * 2 + 1.5f;

            XMFLOAT2 delta(deltaX * sampleOffset, deltaY * sampleOffset);

            // Store texture coordinate offsets for the positive and negative taps.
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 1].x = delta.x;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 1].y = delta.y;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 2].x = -delta.x;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 2].y = -delta.y;
        }

        // Normalize the list of sample weightings, so they will always sum to one.
        for (int i = 0; i < BLUR_SAMPLE_COUNT; i++)
        {
            blurCBuffer.SampleOffsetsAndWeights[i].z /= totalWeights;
        }

        // Update the blur cbuffer with the horizontal blur values.
        d3dDeviceContext->UpdateSubresource(blurD3DBuffer, 0, nullptr, &blurCBuffer, 0, 0);

        d3dDeviceContext->OMSetRenderTargets(1, renderTargetTwo.GetRtv(), nullptr);

        auto context = d3dDeviceContext;
        auto pixelShader = blurPixelShader;
        auto buffer = blurD3DBuffer;

        spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states.Opaque(), nullptr, states.DepthNone(), nullptr, [context, pixelShader, buffer]()
        {
            context->PSSetShader(pixelShader, nullptr, 0);
            context->PSSetConstantBuffers(0, 1, &buffer);
        });
        spriteBatch->Draw(renderTargetOne.GetSrv(), DirectX::XMFLOAT2(0.0f, 0.0f), nullptr);
        spriteBatch->End();

        // Unbind m_renderTargetOne from the graphics pipeline so we can bind it as a render target in the next pass without any warnings.
        context->PSSetShaderResources(0, 1, nullSRV);
    }

    // BLUR VERTICALLY
    {
        float gaussian;
        float offset = 0.0f;
        float deltaX;
        float deltaY;

        deltaX = 0.0f;
        deltaY = 1.0f / renderTargetOne.GetHeight();

        // sigma is the blur amount.
        auto sigmaSquared = 4.0f * 4.0f;

        gaussian = static_cast<float>((1.0 / sqrtf(2.0f * XM_PI * sigmaSquared)) *
            expf(-(offset * offset) / (2 * sigmaSquared)));

        blurCBuffer.SampleOffsetsAndWeights[0].x = 0.0f;
        blurCBuffer.SampleOffsetsAndWeights[0].y = 0.0f;
        blurCBuffer.SampleOffsetsAndWeights[0].z = gaussian;
        int sampleCount = BLUR_SAMPLE_COUNT;

        // Running total of weights.
        float totalWeights = gaussian;

        // Add pairs of additional sample taps, positioned
        // along a line in both directions from the center.
        for (int i = 0; i < sampleCount / 2; i++)
        {
            // Store weights for the positive and negative taps.
            offset = static_cast<float>(i + 1);

            gaussian = static_cast<float>((1.0 / sqrtf(2.0f * XM_PI * sigmaSquared)) *
                expf(-(offset * offset) / (2 * sigmaSquared)));

            float weight = gaussian;

            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 1].z = weight;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 2].z = weight;

            totalWeights += weight * 2;

            // To get the maximum amount of blurring from a limited number of
            // pixel shader samples, we take advantage of the bilinear filtering
            // hardware inside the texture fetch unit. If we position our texture
            // coordinates exactly halfway between two texels, the filtering unit
            // will average them for us, giving two samples for the price of one.
            // This allows us to step in units of two texels per sample, rather
            // than just one at a time. The 1.5 offset kicks things off by
            // positioning us nicely in between two texels.
            float sampleOffset = i * 2 + 1.5f;

            XMFLOAT2 delta(deltaX * sampleOffset, deltaY * sampleOffset);

            // Store texture coordinate offsets for the positive and negative taps.
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 1].x = delta.x;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 1].y = delta.y;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 2].x = -delta.x;
            blurCBuffer.SampleOffsetsAndWeights[i * 2 + 2].y = -delta.y;
        }

        // Normalize the list of sample weightings, so they will always sum to one.
        for (int i = 0; i < BLUR_SAMPLE_COUNT; i++)
        {
            blurCBuffer.SampleOffsetsAndWeights[i].z /= totalWeights;
        }

        d3dDeviceContext->UpdateSubresource(blurD3DBuffer, 0, nullptr, &blurCBuffer, 0, 0);

        d3dDeviceContext->OMSetRenderTargets(1, renderTargetOne.GetRtv(), nullptr);

        auto context = d3dDeviceContext;
        auto pixelShader = blurPixelShader;
        auto buffer = blurD3DBuffer;
        spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred, states.Opaque(), nullptr, states.DepthNone(), nullptr, [context, pixelShader, buffer]()
        {
            context->PSSetShader(pixelShader, nullptr, 0);
            context->PSSetConstantBuffers(0, 1, &buffer);
        });
        spriteBatch->Draw(renderTargetTwo.GetSrv(), DirectX::XMFLOAT2(0.0f, 0.0f), nullptr);
        spriteBatch->End();
        context->PSSetShaderResources(0, 1, nullSRV);
    }

    // Final Pass
    {
        d3dDeviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

        d3dDeviceContext->UpdateSubresource(combineD3DBuffer, 0, nullptr, &bloomCombineCBuffer, 0, 0);

        auto context = d3dDeviceContext;
        auto pixelShader = combinePixelShader;
        auto buffer = combineD3DBuffer;
        auto secondTexture = sceneRenderTarget.GetSrv();

        // The combine operation uses a second texture, the base scene, and performs all blending between the two within the pixel shader itself. So we can still use an opaque
        // state and we just need to bind the base scene (stored in m_sceneRenderTarget) as a second texture (SpriteBatch binds the texture it is drawing as the first shader resource).
        spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred, states.Opaque(), nullptr, states.DepthNone(), nullptr, [context, pixelShader, buffer, secondTexture]()
        {
            context->PSSetShader(pixelShader, nullptr, 0);
            context->PSSetConstantBuffers(0, 1, &buffer);
            context->PSSetShaderResources(1, 1, &secondTexture);
        });

        //// Make sure to scale everything to fit the back buffer.
        RECT rect = { 0L, 0L, static_cast<LONG>(SGS2D::Viewport::GetInstance()->GetWidth()), static_cast<LONG>(SGS2D::Viewport::GetInstance()->GetHeight()) };
        spriteBatch->Draw(renderTargetOne.GetSrv(), rect, nullptr);
        spriteBatch->End();
        context->PSSetShaderResources(0, 1, nullSRV);
        context->PSSetShaderResources(1, 1, nullSRV);
    }
    }

    // Draw HUD

    spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Texture, states.Additive());

    if (!PlayerStatus::GetInstance()->IsGameOver())
    {
        spriteFont->DrawString(spriteBatch.get(), (L"Lives: " + std::to_wstring(PlayerStatus::GetInstance()->GetLives())).c_str(), Vector2(10, 10), Colors::White, 0.0, Vector2(0, 0), 0.5);
        spriteFont->DrawString(spriteBatch.get(), (L"Score: " + std::to_wstring(PlayerStatus::GetInstance()->GetScore())).c_str(), Vector2(10, 40), Colors::White, 0.0, Vector2(0, 0), 0.5);
        spriteFont->DrawString(spriteBatch.get(), (L"Multiplier: " + std::to_wstring(PlayerStatus::GetInstance()->GetMultiplier())).c_str(), Vector2(10, 70), Colors::White, 0.0, Vector2(0, 0), 0.5);
    }
    else
    {
        spriteFont->DrawString(spriteBatch.get(), L"GAME OVER MAN", Vector2((SGS2D::Viewport::GetInstance()->GetWidth() / 2.0f) - 65.0f, (SGS2D::Viewport::GetInstance()->GetHeight() / 2.0f) - 20.0f), Colors::White, 0.0, Vector2(0, 0), 0.5);
    }

	fpsCounter->Draw(*spriteBatch, *spriteFont, Vector2(10, SGS2D::Viewport::GetInstance()->GetHeight() - 100));
    spriteFont->DrawString(spriteBatch.get(), (L"Entity Count: " + std::to_wstring(EntityManager::GetInstance()->GetEntityCount())).c_str(), Vector2(10, SGS2D::Viewport::GetInstance()->GetHeight() - 70), Colors::White, 0.0, Vector2(0, 0), 0.5);
    spriteFont->DrawString(spriteBatch.get(), (L"Particle Count: " + std::to_wstring(ParticleManager::GetInstance()->ParticleCount())).c_str(), Vector2(10, SGS2D::Viewport::GetInstance()->GetHeight() - 40), Colors::White, 0.0, Vector2(0, 0), 0.5);
	
	spriteBatch->End();

	// Present the backbuffer
    swapChain->Present(0, 0);
}