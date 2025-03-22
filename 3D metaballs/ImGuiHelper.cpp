#include "ImGuiHelper.h"

void SetupImGui(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(windowHandle);
	ImGui_ImplDX11_Init(device, context);
}

void StartImGuiFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}



void EndImGuiFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

ImVec4 selectedColor = ImVec4(0.898f, 0.0f, 1.0f, 1.0f);  // Initially selected color (green)
ImVec4 sphereCenter = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
float radius = 0.75;

ImVec2 animationPosX = ImVec2(0.0f, 0.0f);  // animationPosX: [0.0, 0.0]

ImVec2 animationPosY = ImVec2(0.0f, 0.0f);  // animationPosY: [0.0, 0.0]
ImVec2 animationPosZ = ImVec2(0.0f, 0.0f);  // animationPosZ: [0.0, 0.0]

bool mapCapacity = false;
int maxBalls = 10;

void ImGuiModifying(ID3D11DeviceContext* context, std::vector<Metaball>& Metaballs,
std::vector<sphereAnimationData>& sphere, int& SphereCounter)
{
	bool begun = ImGui::Begin("Modifiers");
	
	if (begun)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // background
		// Slightly lighter shade of black for hover (a subtle highlight)
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));  // Subtle gray for hover
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.8f, 1.0f));  // Active button background
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));  // White text
		if (ImGui::Button("Add Sphere")) {

			
			if (Metaballs.size() >= maxBalls)
			{
				mapCapacity = true;
				
			}
			else {

				Metaball meatBall;
				meatBall.center = { sphereCenter.x, sphereCenter.y, sphereCenter.z };
				meatBall.color = { selectedColor.x, selectedColor.y, selectedColor.z, selectedColor.w };
				meatBall.radius = radius;

				Metaballs.push_back(meatBall);

				// animation
				sphereAnimationData sphereData;
				sphereData.sphereCenter = meatBall.center;
				if (animationPosX.x != 0.0 && animationPosX.y != 0.0)
				{
					sphereData.inversX = true;
				}
				if (animationPosY.x != 0.0 && animationPosX.y != 0.0)
				{
					sphereData.inversY = true;
				}
				if (animationPosZ.x != 0.0 && animationPosZ.y != 0.0)
				{
					sphereData.inversZ = true;
				}


				sphereData.animationPosX = { animationPosX.x, animationPosX.y };
				sphereData.animationPosY = { animationPosY.x, animationPosY.y };
				sphereData.animationPosZ = { animationPosZ.x, animationPosZ.y };
				sphere.push_back(sphereData);
				SphereCounter++;
			}
			


			
		}
		ImGui::PopStyleColor(4);  // 4 because we pushed 4 colors

		if (mapCapacity)
		{
			// Set the cursor position to the same location as the "Add" button
			ImVec2 buttonPos = ImGui::GetItemRectMin();  // Get button position
			ImVec2 buttonSize = ImGui::GetItemRectSize();  // Get button size
			//ImGui::SetCursorPos(ImVec2(buttonPos.x + 1.0, buttonPos.y)); // Offset a bit down
			std::string errorMsg = "Error: Metaballs cannot exceed: " + std::to_string(maxBalls);
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMsg.c_str());
		}
		

		if (ImGui::ColorEdit4("Select Color", (float*)&selectedColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB))
		{
			// Color has been selected, and `selectedColor` is updated
		}


		if (ImGui::InputFloat3("Spheres Center", (float*)&sphereCenter, "%.2f"))
		{
			// center has been selected
		}

		if (ImGui::InputFloat("Spheres Radius", &radius))
		{
			// center has been selected
		}

		ImGui::Text("Animation Settings");


		if (ImGui::InputFloat2("animationPosX", (float*)&animationPosX))
		{
			// animationPosX has been selected
		}

		if (ImGui::InputFloat2("animationPosY", (float*)&animationPosY))
		{
			// animationPosY has been selected
		}

		if (ImGui::InputFloat2("animationPosZ", (float*)&animationPosZ))
		{
			// animationPosZ has been selected
		}

		

		

	}

	ImGui::End();
}
