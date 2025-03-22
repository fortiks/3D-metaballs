#pragma once
#include <d3d11_4.h>

bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device,
	ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv,
	ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport);
