#include "VertexBufferD3D11.h"

VertexBufferD3D11::VertexBufferD3D11(ID3D11Device* device, UINT sizeOfVertex, UINT nrOfVerticesInBuffer, void* vertexData)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeOfVertex * nrOfVerticesInBuffer;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	this->nrOfVertices = nrOfVerticesInBuffer;
	this->vertexSize = sizeOfVertex;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &buffer);
}

VertexBufferD3D11::~VertexBufferD3D11()
{
	if (buffer)
	{
		buffer->Release();
	}
		
}




VertexBufferD3D11& VertexBufferD3D11::operator=(VertexBufferD3D11&& other) noexcept
{
	if (this != &other)
	{
		if (buffer)
		{
			buffer->Release();
		}

		buffer = other.buffer;
		nrOfVertices = other.nrOfVertices;
		vertexSize = other.vertexSize;

		other.buffer = nullptr;
		other.nrOfVertices = 0;
	}
	return *this;
}

void VertexBufferD3D11::Initialize(ID3D11Device* device, UINT sizeOfVertex, UINT nrOfVerticesInBuffer, void* vertexData)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeOfVertex * nrOfVerticesInBuffer;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	this->nrOfVertices = nrOfVerticesInBuffer;
	this->vertexSize = sizeOfVertex;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &buffer);
}

UINT VertexBufferD3D11::GetNrOfVertices() const
{
	return this->nrOfVertices;
}

UINT VertexBufferD3D11::GetVertexSize() const
{
	return this->vertexSize;
}

ID3D11Buffer* VertexBufferD3D11::GetBuffer() const
{
	return this->buffer;
}
