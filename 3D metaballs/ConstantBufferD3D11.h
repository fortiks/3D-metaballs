#pragma once
#include <d3d11_4.h>
#include <DirectXMath.h>
#include <array>
#include <string>


struct SimpleVertex
{
	float pos[3];

	SimpleVertex(const std::array<float, 3>& position)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
		}
		
	}
};

struct cameraConstData {
	DirectX::XMFLOAT3 cameraPosition;
	float padding1;
	DirectX::XMFLOAT3 lookAt_target;
	float padding2;
	DirectX::XMFLOAT3 up_vector;
	float padding3;
};

struct SphereBuffer {
	DirectX::XMFLOAT3 sphereCenter;
};

struct Metaball
{
	
	DirectX::XMFLOAT3 center; // Position of the metaball (x, y, z)
	float radius; // Radius of the metaball
	DirectX::XMFLOAT4 color;
	Metaball() = default;
	Metaball(DirectX::XMFLOAT3 center, float radius, DirectX::XMFLOAT4 color)
	{
		this->center = center;
		this->radius = radius;
		this->color = color;
	};
};

struct resolutionBuffer
{
	DirectX::XMFLOAT2 resolution;
	int SphereCounter = 0;
	float padding;

};

struct sphereAnimationData {
	DirectX::XMFLOAT3 sphereCenter = { 0.0f, 0.0f, 0.0f }; // Center : [0.0,0.0,0.0]
	DirectX::XMFLOAT2 animationPosX = { 0.0f, 0.0f };  // animationPosX: [0.0, 0.0]
	DirectX::XMFLOAT2 animationPosY = { 0.0f, 0.0f };  // animationPosY: [0.0, 0.0]
	DirectX::XMFLOAT2 animationPosZ = { 0.0f, 0.0f };  // animationPosZ: [0.0, 0.0]
	bool inversX = false;  // inversX: false
	bool inversY = false;  // inversY: false
	bool inversZ = false;  // inversZ: false
};

class ConstantBufferD3D11
{
private:
	ID3D11Buffer* buffer = nullptr;
	size_t bufferSize = 0;

public:
	ConstantBufferD3D11() = default;
	ConstantBufferD3D11(ID3D11Device* device, size_t byteSize, void* initialData = nullptr);
	~ConstantBufferD3D11();

	ConstantBufferD3D11(const ConstantBufferD3D11& other) = delete;
	ConstantBufferD3D11& operator=(const ConstantBufferD3D11& other) = delete;

	ConstantBufferD3D11(ConstantBufferD3D11&& other) noexcept; // Move constructor
	ConstantBufferD3D11& operator=(ConstantBufferD3D11&& other) noexcept; // Move assignment operator

	void Initialize(ID3D11Device* device, size_t byteSize, void* initialData = nullptr);

	size_t GetSize() const;
	ID3D11Buffer* GetBuffer() const;

	void UpdateBuffer(ID3D11DeviceContext* context, void* data, size_t dataSize);
};