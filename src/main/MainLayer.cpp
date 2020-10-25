#include "gspch.h"
#include "MainLayer.h"

#include <imgui.h>
#include <Hazel.h>
#include <libusbp.hpp>

#include <mutex>

#include "util/ImGuiConsole.h"
#include "util/SerialPort.h"


int radioVID = 0x0403;//0x0403
int radioPID = 0x6001;//0x6001

int arduinoVID = 0x2341;//0x0403
int arduinoPID = 0x0042;//0x6001

void MainLayer::OnAttach()
{
	m_Port = new SerialPort(*this);
	m_GroundStation = GetDefaultCoreModule();

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

const char* dialogID = "Test Dialog";

struct DialogData
{
	DialogData(char id, const std::vector<std::string>& options, std::promise<std::string>* promise) : ID{ id, 0x00 }, Options(options), Promise(promise), NewItem(true) {}

	char ID[2];
	std::vector<std::string> Options;
	std::promise<std::string>* Promise;
	bool NewItem;
};


static std::vector<DialogData> s_Dialogs;
static std::mutex s_DialogsMutex;

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
	
	s_DialogsMutex.lock();
	bool done = false;
	for (auto it = s_Dialogs.begin(); it != s_Dialogs.end(); it++)
	{
		DialogData& dialog = *it;

		if (dialog.NewItem)
		{
			ImGui::OpenPopup(dialog.ID);
			HZ_INFO("Opening dialog!");
			dialog.NewItem = false;
		}
		
		if (ImGui::BeginPopupModal(dialog.ID, nullptr, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::Text("Select an option:");
			
			for (const std::string& option : dialog.Options)
			{
				if (ImGui::Button(option.c_str()))
				{
					ImGui::CloseCurrentPopup();
					it->Promise->set_value(option);
					//Break out of both loops
					done = true;
				}

			}
			ImGui::EndPopup();
		}
		if (done)
		{
			it = s_Dialogs.erase(it);
			break;
		}
	}
	s_DialogsMutex.unlock();

	console.Render();
}

void MainLayer::OpenDialog(const std::vector<std::string>& options, std::promise<std::string>* promise)
{
	std::scoped_lock(s_DialogsMutex);
	s_Dialogs.emplace_back('A' + s_Dialogs.size(), options, promise);
}


MainLayer::~MainLayer()
{
}

void MainLayer::HandleGPS(const std::string& sentence)
{
	HZ_INFO("GPS: {}", sentence);
}
