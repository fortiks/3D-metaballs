#include "ShaderD3D11.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cassert>

ShaderD3D11::~ShaderD3D11()
{
    if (shaderBlob) shaderBlob->Release();

    switch (type)
    {
    case ShaderType::VERTEX_SHADER:
        if (shader.vertex) shader.vertex->Release();
        break;
    case ShaderType::HULL_SHADER:
        if (shader.hull) shader.hull->Release();
        break;
    case ShaderType::DOMAIN_SHADER:
        if (shader.domain) shader.domain->Release();
        break;
    case ShaderType::GEOMETRY_SHADER:
        if (shader.geometry) shader.geometry->Release();
        break;
    case ShaderType::PIXEL_SHADER:
        if (shader.pixel) shader.pixel->Release();
        break;
    case ShaderType::COMPUTE_SHADER:
        if (shader.compute) shader.compute->Release();
        break;
    }
}

ShaderD3D11::ShaderD3D11(ID3D11Device* device, ShaderType shaderType, const void* dataPtr, size_t dataSize) // constructor using Bytecode  in Memory
{
    Initialize(device, shaderType, dataPtr, dataSize);

}

ShaderD3D11::ShaderD3D11(ID3D11Device* device, ShaderType shaderType, std::wstring csoPath) // complied shader file .cso
{
    Initialize(device, shaderType, csoPath);
}

void ShaderD3D11::Initialize(ID3D11Device* device, ShaderType shaderType, const void* dataPtr, size_t dataSize)
{
    type = shaderType;
    std::string typeString = "";

    HRESULT hr;
    switch (type)
    {
    case ShaderType::VERTEX_SHADER:
        typeString = "VERTEX_SHADER";
        hr = device->CreateVertexShader(dataPtr, dataSize, nullptr, &shader.vertex);
        break;
    case ShaderType::HULL_SHADER:
        typeString = "HULL_SHADER";
        hr = device->CreateHullShader(dataPtr, dataSize, nullptr, &shader.hull);
        break;
    case ShaderType::DOMAIN_SHADER:
        typeString = "DOMAIN_SHADER";
        hr = device->CreateDomainShader(dataPtr, dataSize, nullptr, &shader.domain);
        break;
    case ShaderType::GEOMETRY_SHADER:
        typeString = "GEOMETRY_SHADER";
        hr = device->CreateGeometryShader(dataPtr, dataSize, nullptr, &shader.geometry);
        break;
    case ShaderType::PIXEL_SHADER:
        typeString = "PIXEL_SHADER";
        hr = device->CreatePixelShader(dataPtr, dataSize, nullptr, &shader.pixel);
        break;
    case ShaderType::COMPUTE_SHADER:
        typeString = "COMPUTE_SHADER";
        hr = device->CreateComputeShader(dataPtr, dataSize, nullptr, &shader.compute);
        break;
    default:
        hr = E_FAIL;
    }

    if (FAILED(hr))
    {
        std::string errorMessage = "Failed to read shader. Type: " + typeString;
        MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_ICONERROR);
    }

    //shaderBlob = nullptr;  // No blob in this case
}
void ShaderD3D11::Initialize(ID3D11Device* device, ShaderType shaderType, std::wstring csoPath)
{
    type = shaderType;

    // Load the compiled shader object (CSO) file
    HRESULT hr = D3DReadFileToBlob(csoPath.c_str(), &shaderBlob);
    if (FAILED(hr))
    {
        MessageBoxA(NULL, "Failed to read shader.", "Error", MB_ICONERROR);
    }

    Initialize(device, shaderType, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
}
ID3D11ComputeShader* ShaderD3D11::GetComputeShader()
{
    return shader.compute;
}
const void* ShaderD3D11::GetShaderByteData() const
{
    assert(shaderBlob && "Shader blob is not available.");
    return shaderBlob->GetBufferPointer();
}
size_t ShaderD3D11::GetShaderByteSize() const
{
    assert(shaderBlob && "Shader blob is not available.");
    return shaderBlob->GetBufferSize();
}

void ShaderD3D11::BindShader(ID3D11DeviceContext * context) const
{
    switch (type)
    {
    case ShaderType::VERTEX_SHADER:
        context->VSSetShader(shader.vertex, nullptr, 0);
        break;
    case ShaderType::HULL_SHADER:
        context->HSSetShader(shader.hull, nullptr, 0);
        break;
    case ShaderType::DOMAIN_SHADER:
        context->DSSetShader(shader.domain, nullptr, 0);
        break;
    case ShaderType::GEOMETRY_SHADER:
        context->GSSetShader(shader.geometry, nullptr, 0);
        break;
    case ShaderType::PIXEL_SHADER:
        context->PSSetShader(shader.pixel, nullptr, 0);
        break;
    case ShaderType::COMPUTE_SHADER:
        context->CSSetShader(shader.compute, nullptr, 0);
        break;
    default:
        throw MessageBoxA(NULL, "Invalid shader type.", "Error", MB_ICONERROR);
    }
}



