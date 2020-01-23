
#include "GroundStation.h"

#include <Hazel/Core/EntryPoint.h>

GroundStation::GroundStation()
{
}

void GroundStation::Update(Hazel::Timestep ts)
{
}

void GroundStation::Render()
{
}

GroundStation::~GroundStation()
{
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

