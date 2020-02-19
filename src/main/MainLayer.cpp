#include "MainLayer.h"

#include <imgui.h>
#include <Hazel.h>
#include <libusbp.hpp>

#include "util/ImGuiConsole.h"

int vendorID = 0x0403;
int productID = 0x6001;

void MainLayer::OnAttach()
{
	HZ_INFO("Pre devices");
	for (libusbp::device& device : libusbp::list_connected_devices())
	{
		try
		{
			HZ_INFO("Device: VID: {}, PID: {}", device.get_vendor_id(), device.get_product_id(), device.get_serial_number());
		}
		catch (const std::exception& error)
		{
			HZ_ERROR("libusbp Error: {}", error.what());
		}
		
	}
	HZ_INFO("Post devices");

/*	libusbp::serial_port port(device, interface_number, composite);
	std::string port_name = port.get_name();
	std::cout << port_name << std::endl;
*/

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
