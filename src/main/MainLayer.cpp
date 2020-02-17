#include "MainLayer.h"

#include <imgui.h>
#include <Hazel.h>
#include <libusb.h>

#include "util/ImGuiConsole.h"

void MainLayer::OnAttach()
{
	libusb_device **devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0){
		libusb_exit(NULL);
		return;
	}

	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8]; 

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}
		
		printf("%x:%x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			printf(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				printf(".%d", path[j]);
		}
		printf("\n");
	}

	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
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
