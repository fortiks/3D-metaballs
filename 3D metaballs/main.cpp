// buffers
#include "VertexBufferD3D11.h"
#include "ConstantBufferD3D11.h"
#include "StructuredBufferD3D11.h"


#include "InputLayoutD3D11.h"
#include "ShaderD3D11.h"
#include "WindowHelper.h"
#include "D3D11Setup.h"
#include "IMGuiHelper.h"


#include <Windows.h>

// time
#include <chrono>
#include <thread> 


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	const UINT WIDTH = 1024;
	const UINT HEIGHT = 576;
	HWND window;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		MessageBoxW(NULL, L"Failed to setup window!", L"Error", MB_ICONERROR);
		return -1;
	}


	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* dsTexture;
	ID3D11DepthStencilView* dsView;
	D3D11_VIEWPORT viewport;

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport))
	{
		MessageBoxW(NULL, L"Failed to setup d3d11!", L"Error", MB_ICONERROR);
		return -1;
	}

	SetupImGui(window, device, immediateContext);
	

	ShaderD3D11 pixelShader;
	pixelShader.Initialize(device,
		ShaderType::PIXEL_SHADER, L"..\\x64\\Debug\\3DMetaBallsPixelShader.cso");

	ShaderD3D11 vertexShader;
	vertexShader.Initialize(device,
		ShaderType::VERTEX_SHADER, L"..\\x64\\Debug\\vertexshader.cso");

	InputLayoutD3D11 inputLayout;
	inputLayout.AddInputElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayout.FinalizeInputLayout(device, vertexShader.GetShaderByteData(), vertexShader.GetShaderByteSize());

	DirectX::XMFLOAT3 position[6] = {
		// First Triangle
		{ -0.5f,  0.5f, 0.0f}, // Top Left
		{  0.5f,  0.5f, 0.0f}, // Top Right
		{  0.5f, -0.5f, 0.0f}, // Bottom Right

		// Second Triangle
		{  0.5f, -0.5f, 0.0f}, // Bottom Right
		{ -0.5f, -0.5f, 0.0f}, // Bottom Left
		{ -0.5f,  0.5f, 0.0f}  // Top Left
	};
		

	VertexBufferD3D11 vertexBuffer(device, sizeof(DirectX::XMFLOAT3), 6, position);

	DirectX::XMMATRIX transform = DirectX::XMMatrixScaling(2,2,0);

	ConstantBufferD3D11 constantVsBuffer(device, sizeof(DirectX::XMMATRIX), &transform);


	cameraConstData cameraData;
	cameraData.cameraPosition = { 0,0,7 };
	cameraData.lookAt_target = { 0,0,0 };
	cameraData.up_vector = { 0,1,0 };

	ConstantBufferD3D11 constantCameraBuffer(device, sizeof(cameraData), &cameraData);
	
	std::vector<Metaball> Metaballs;
	
	Metaballs.push_back(Metaball({ 0.0,0.0,0.0 }, 0.75, {1.0, 0.0, 0.0, 1.0}));
	Metaballs.push_back(Metaball({ 1.0, 0.0, 0.0 }, 1.0, { 1.0, 1.0, 0.0, 1.0}));
	Metaballs.push_back(Metaball({ 1.0, 1.0, 0.0 }, 0.75, { 0.0, 0.0, 1.0, 1.0}));


	StructuredBufferD3D11 MetaballsBuffer(device, sizeof(Metaball), 10, Metaballs.data()); // set up for 10 balls

	resolutionBuffer resolutionData;
	resolutionData.resolution = { WIDTH, HEIGHT };
	resolutionData.SphereCounter = 3;
	ConstantBufferD3D11 constatResolutionBuffer(device, sizeof(resolutionData), &resolutionData);

	std::vector<sphereAnimationData> sphere(3); // Defualt start

	// Sphere 0
	
	sphere[0].sphereCenter = Metaballs[0].center;
	sphere[0].animationPosX = DirectX::XMFLOAT2(6.5f, -6.5f);
	sphere[0].animationPosY = DirectX::XMFLOAT2(1.0f, -1.0f); 

	// Sphere 1
	sphere[1].sphereCenter = Metaballs[1].center;
	sphere[1].animationPosX = DirectX::XMFLOAT2(6.5f, -6.5f);
	sphere[1].inversX = true;

	
	// Sphere 2
	sphere[2].sphereCenter = Metaballs[2].center;
	sphere[2].animationPosX = DirectX::XMFLOAT2(4.0f, -4.0f);
	sphere[2].animationPosY = DirectX::XMFLOAT2(3.0f, -3.0f);
	sphere[2].animationPosZ = DirectX::XMFLOAT2(1.0f, -3.0f);
	sphere[2].inversX = true;
	sphere[2].inversZ = true;

	MSG msg = { };
	std::chrono::duration<float> dt; // time
	auto startTime = std::chrono::high_resolution_clock::now();
	
	float speed = 1.0f;
	auto endTime = std::chrono::high_resolution_clock::now();
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		StartImGuiFrame();

		ImGuiModifying(immediateContext, Metaballs, sphere, resolutionData.SphereCounter);
		constatResolutionBuffer.UpdateBuffer(immediateContext, &resolutionData, sizeof(resolutionData));

		// Movement 
		dt = endTime - startTime;
		startTime = std::chrono::high_resolution_clock::now();
		// Loop to animate each sphere 
		for (int i = 0; i < resolutionData.SphereCounter; i++) {

			// Update X coordinate
			if (sphere[i].sphereCenter.x < sphere[i].animationPosX.x && !sphere[i].inversX) {

				sphere[i].sphereCenter.x += dt.count() * speed;

				if (sphere[i].sphereCenter.x >= sphere[i].animationPosX.x) {
					sphere[i].inversX = true;
				}
			}

			if (sphere[i].sphereCenter.x > sphere[i].animationPosX.y && sphere[i].inversX) {
				sphere[i].sphereCenter.x -= dt.count() * speed;

				if (sphere[i].sphereCenter.x <= sphere[i].animationPosX.y) {
					sphere[i].inversX = false;
				}
			}

			// Update Y coordinate
			if (sphere[i].sphereCenter.y < sphere[i].animationPosY.x && !sphere[i].inversY) {
				sphere[i].sphereCenter.y += dt.count() * speed;

				if (sphere[i].sphereCenter.y >= sphere[i].animationPosY.x) {
					sphere[i].inversY = true;
				}
			}

			if (sphere[i].sphereCenter.y > sphere[i].animationPosY.y && sphere[i].inversY) {
				sphere[i].sphereCenter.y -= dt.count() * speed;

				if (sphere[i].sphereCenter.y <= sphere[i].animationPosY.y) {
					sphere[i].inversY = false;
				}
			}

			// Update Z coordinate
			if (sphere[i].sphereCenter.z < sphere[i].animationPosZ.x && !sphere[i].inversZ) {
				sphere[i].sphereCenter.z += dt.count() * speed;

				if (sphere[i].sphereCenter.z >= sphere[i].animationPosZ.x) {
					sphere[i].inversZ = true;
				}
			}

			if (sphere[i].sphereCenter.z > sphere[i].animationPosZ.y && sphere[i].inversZ) {
				sphere[i].sphereCenter.z -= dt.count() * speed;

				if (sphere[i].sphereCenter.z <= sphere[i].animationPosZ.y) {
					sphere[i].inversZ = false;
				}
			}
			Metaballs[i].center = sphere[i].sphereCenter;
			
		}

		
		MetaballsBuffer.UpdateBuffer(immediateContext, Metaballs.data());
		

		// === Render === 
		float clearColour[4] = { 0, 0, 0, 0 };
		immediateContext->ClearRenderTargetView(rtv, clearColour);
		immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);


		ID3D11Buffer* VertexBuffer = vertexBuffer.GetBuffer();
		ID3D11Buffer* ConstantBuffer = constantVsBuffer.GetBuffer();

		// 
		ID3D11Buffer* CameraBuffer = constantCameraBuffer.GetBuffer();
		immediateContext->PSSetConstantBuffers(0, 1, &CameraBuffer);

		ID3D11Buffer* ResolutionBuffer = constatResolutionBuffer.GetBuffer();
		immediateContext->PSSetConstantBuffers(1, 1, &ResolutionBuffer);

		ID3D11ShaderResourceView* SpheresSRV = MetaballsBuffer.GetSRV();

		if (SpheresSRV == NULL)
		{
			throw std::runtime_error("SpheresSRV is null");
		}
		
		immediateContext->PSSetShaderResources(0, 1, &SpheresSRV);
	

		UINT stride = vertexBuffer.GetVertexSize();
		UINT offset = 0;
		immediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		immediateContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
		immediateContext->IASetInputLayout(inputLayout.GetInputLayout());
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		vertexShader.BindShader(immediateContext);
		pixelShader.BindShader(immediateContext);

		immediateContext->RSSetViewports(1, &viewport);
		immediateContext->OMSetRenderTargets(1, &rtv, dsView);

		immediateContext->Draw(6, 0);

		EndImGuiFrame();
		swapChain->Present(1, 0);

		endTime = std::chrono::high_resolution_clock::now();
		
	}

	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}