#include "MainLayer.h"

#include <imgui.h>

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

void MainLayer::OnImGuiRender()
{
	//ImGui::PlotLines()
	ImGui::Begin("Test");
	ImGui::Button("test2");
	ImGui::End();
}

MainLayer::~MainLayer()
{
}
