#include "MainLayer.h"

#include <imgui.h>
#include <Hazel.h>
#include <libusbp.hpp>

#include "util/ImGuiConsole.h"

int radioVID = 0x0403;//0x0403
int radioPID = 0x6001;//0x6001

int arduinoVID = 0x2341;//0x0403
int arduinoPID = 0x0042;//0x6001

void MainLayer::OnAttach()
{
	try
	{
		libusbp::device device = libusbp::find_device_with_vid_pid(radioVID, radioPID);
		libusbp::serial_port port(device, 0, false);
		HZ_INFO("Opened Radio serial port");
		std::string port_name = port.get_name();
		HZ_INFO("Radio Path: {}", port_name);
	}
	catch (const std::exception& error)
	{
		HZ_ERROR("libusbp Error: {}", error.what());
	}
	
	try
	{
		libusbp::device device = libusbp::find_device_with_vid_pid(arduinoVID, arduinoPID);
		libusbp::serial_port port(device, 0, false);
		HZ_INFO("Opened Arduino serial port");
		std::string port_name = port.get_name();
		HZ_INFO("Arduino Path: {}", port_name);
	}
	catch (const std::exception & error)
	{
		HZ_ERROR("libusbp Error: {}", error.what());
	}



}

void MainLayer::OnDetach()
{
}

void MainLayer::OnUpdate(Hazel::Timestep ts)
{
}

ImGuiConsole console("Test Console");


void MainLayer::OnEvent(Hazel::Event* event)
{
	Hazel::EventDispatcher dis(event);
	dis.Dispatch<Hazel::KeyPressedEvent>([&](Hazel::KeyPressedEvent* event) -> bool {
		if (event->GetKeyCode() == HZ_KEY_L)
		{
			console.AddData("line, line, test me. ");
			return true;
		}
		else if (event->GetKeyCode() == HZ_KEY_N)
		{
			console.AddData("\n");
			return true;
		}
		return false;
	});
}

void MainLayer::Render()
{
}

float getData(void* data, int idx)
{
	return ((float*)data)[idx];
}

void SerialLog()
{

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

	console.Render();
}

MainLayer::~MainLayer()
{
}
