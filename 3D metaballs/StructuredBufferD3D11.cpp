#include "StructuredBufferD3D11.h"

StructuredBufferD3D11::StructuredBufferD3D11(ID3D11Device* device, UINT sizeOfElement, size_t nrOfElementsInBuffer, 
	void* bufferData, bool dynamic, bool hasUAV)
{
	Initialize(device, sizeOfElement, nrOfElementsInBuffer, bufferData, dynamic, hasUAV);
}

StructuredBufferD3D11::~StructuredBufferD3D11()
{
	if (srv) srv->Release();
	if (buffer) buffer->Release();
    if (uav) uav->Release();
}

void StructuredBufferD3D11::Initialize(ID3D11Device* device, UINT sizeOfElement, size_t nrOfElementsInBuffer,
        void* bufferData, bool dynamic, bool hasUAV)
{
    this->elementSize = sizeOfElement;
    this->nrOfElements = nrOfElementsInBuffer;
    this->dynamic = dynamic;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeOfElement * static_cast<UINT>(nrOfElementsInBuffer);
    bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.BindFlags |= hasUAV ? D3D11_BIND_UNORDERED_ACCESS : 0;
    bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = sizeOfElement;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = bufferData;

    HRESULT hr = device->CreateBuffer(&bufferDesc, bufferData ? &initData : nullptr, &buffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create structured buffer.");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = nrOfElementsInBuffer;

    hr = device->CreateShaderResourceView(buffer, &srvDesc, &srv);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create SRV for structured buffer.");
    }

    if (hasUAV)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = nrOfElementsInBuffer;
        uavDesc.Buffer.Flags = 0;
       
        hr = device->CreateUnorderedAccessView(buffer, &uavDesc, &uav);
    
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create uav for structured buffer");
        }
    }
}

void StructuredBufferD3D11::UpdateBuffer(ID3D11DeviceContext* context, void* data)
{
    if (!dynamic) throw std::runtime_error("Cannot update a non-dynamic buffer.");

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) throw std::runtime_error("Failed to map buffer for updating.");

    memcpy(mappedResource.pData, data, elementSize * nrOfElements);
    context->Unmap(buffer, 0);
}

UINT StructuredBufferD3D11::GetElementSize() const
{
    return elementSize;
}

size_t StructuredBufferD3D11::GetNrOfElements() const
{
    return nrOfElements;
}

ID3D11ShaderResourceView* StructuredBufferD3D11::GetSRV() const
{
    return srv;
}

ID3D11UnorderedAccessView* StructuredBufferD3D11::GetUAV() const
{
    return uav;
}

ID3D11Buffer* StructuredBufferD3D11::GetBuffer() const
{
    return buffer;
}

