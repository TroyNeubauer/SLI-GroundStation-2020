#include "gspch.h"

#include "GroundStation.h"

#include "MainLayer.h"
#include "util/SerialPort.h"

#ifndef GROUND_STATION_TEST//A seperate main() is provided by the Test project
	#include <Hazel/Core/EntryPoint.h>
#endif

GroundStation::GroundStation()
{
	GSTime::InitClock();
	SerialPort::GetPort().Init();


	m_StartTime = GSTime::Now();

	PushOverlay(new MainLayer());
	PushOverlay(new Hazel::DebugLayer());
}

void GroundStation::Update(Hazel::Timestep ts)
{

}

void GroundStation::Render()
{
	Hazel::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	Hazel::RenderCommand::Clear();
}

GroundStation::~GroundStation()
{
	SerialPort::GetPort().Close();
}



Hazel::Application* HZCreateApplication(int argc, char** argv)
{
	return new GroundStation();
}

void HZGetApplicationInfo(Hazel::ApplicationInfo* info)
{
	info->Name = "Ground station";
	info->Publisher = "OHS Rocketry";
	info->Version = "v0.1 beta";
	info->VersionInt = 0;
}

