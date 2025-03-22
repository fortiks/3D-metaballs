#pragma once
#include <d3d11.h>
#include <Windows.h>
#include "ConstantBufferD3D11.h"
#include <vector>

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"


void SetupImGui(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* context);

void StartImGuiFrame();


void ImGuiModifying(ID3D11DeviceContext* context, std::vector<Metaball>& Metaballs,
std::vector<sphereAnimationData>& sphere, int& SphereCounter);

void EndImGuiFrame();