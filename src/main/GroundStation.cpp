
#include "GroundStation.h"

#ifndef GROUND_STATION_TEST//A seperate main() is provided by the Test project
	#include <Hazel/Core/EntryPoint.h>
#endif

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

