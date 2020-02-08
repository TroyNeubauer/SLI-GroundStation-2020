#include "MainLayer.h"
#include <imgui.h>
#include <Hazel.h>

void MainLayer::OnAttach()
{
}

void MainLayer::OnDetach()
{
}

void MainLayer::OnUpdate(Hazel::Timestep ts)
{
}

void MainLayer::OnEvent(Hazel::Event* event)
{
}

void MainLayer::Render()
{
}

float getData(void* data, int idx)
{
	return ((float*)data)[idx];
}

void MainLayer::OnImGuiRender()
{
	float arr[] = { 1,2,1,-1,5 };
	ImGui::PlotLines("Graph", getData, arr, sizeof(arr) / sizeof(arr[0]));
	ImGui::Begin("Test");
	if (ImGui::Button("test2")) {
		HZ_INFO("clicked");
	};
	ImGui::End();
	ImGui::Begin("Test1");
	ImGui::Button("test2");
	ImGui::End();
}

MainLayer::~MainLayer()
{
}
