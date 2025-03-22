#include "InputLayoutD3D11.h"

InputLayoutD3D11::~InputLayoutD3D11()
{
    if (inputLayout)
    {
        inputLayout->Release();
        inputLayout = nullptr;
    }
}

void InputLayoutD3D11::AddInputElement(const std::string& semanticName, DXGI_FORMAT format)
{
    D3D11_INPUT_ELEMENT_DESC element = {};
    element.SemanticName = semanticName.c_str(); // Semantic name (e.g., POSITION, NORMAL).
    element.SemanticIndex = 0; // Index for semantics with the same name.
    element.Format = format; // Data format (e.g., DXGI_FORMAT_R32G32B32_FLOAT).
    element.InputSlot = 0; // Input slot index.
    element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // Automatically determine offset.
    element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // Data is per-vertex.
    element.InstanceDataStepRate = 0; // No instancing.

    semanticNames.push_back(semanticName); // Store the semantic name.
    elements.push_back(element); // Add the element descriptor.
}

void InputLayoutD3D11::FinalizeInputLayout(ID3D11Device* device, const void* vsDataPtr, size_t vsDataSize)
{
    // Update semantic names for each element.
    for (size_t i = 0; i < elements.size(); ++i)
    {
        elements[i].SemanticName = semanticNames[i].c_str();
    }

    // Create the input layout using the provided vertex shader bytecode.
    HRESULT hr = device->CreateInputLayout(elements.data(), static_cast<UINT>(elements.size()), vsDataPtr, vsDataSize, &inputLayout);
    if (FAILED(hr))
    {
        MessageBoxA(NULL, "Error: Failed to create input layout.", "Error", MB_ICONERROR);
        
    }
}

ID3D11InputLayout* InputLayoutD3D11::GetInputLayout() const
{
    return inputLayout;
}
