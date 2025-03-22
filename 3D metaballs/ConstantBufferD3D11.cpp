#include "ConstantBufferD3D11.h"
#include <iostream>

ConstantBufferD3D11::ConstantBufferD3D11(ID3D11Device* device, size_t byteSize, void* initialData)
{
	bufferSize = byteSize;
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth = byteSize;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&cBufferDesc, &data, &this->buffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Error creating constantbuffer.");
	}
}

ConstantBufferD3D11::~ConstantBufferD3D11()
{
	buffer->Release();
}

ConstantBufferD3D11::ConstantBufferD3D11(ConstantBufferD3D11&& other) noexcept
{
	buffer = other.buffer;
	bufferSize = other.bufferSize;
	other.buffer = nullptr;
}

ConstantBufferD3D11& ConstantBufferD3D11::operator=(ConstantBufferD3D11&& other) noexcept
{
	if (this != &other)
	{
		delete buffer;
		buffer = other.buffer;
		other.buffer = nullptr;

	}
	return *this;
}

void ConstantBufferD3D11::Initialize(ID3D11Device* device, size_t byteSize, void* initialData)
{
	bufferSize = byteSize;
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth = byteSize;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&cBufferDesc, &data, &this->buffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Error creating constantbuffer.");
	
	}
}

size_t ConstantBufferD3D11::GetSize() const
{
	return this->bufferSize;
}

ID3D11Buffer* ConstantBufferD3D11::GetBuffer() const
{
	return this->buffer;
}

void ConstantBufferD3D11::UpdateBuffer(ID3D11DeviceContext* context, void* data, size_t dataSize)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	ZeroMemory(&msr, sizeof(D3D11_MAPPED_SUBRESOURCE));
	context->Map(this->buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	std::memcpy(msr.pData, data, dataSize);
	context->Unmap(this->buffer, 0);
}
