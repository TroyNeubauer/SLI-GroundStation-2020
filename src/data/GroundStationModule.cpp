#include "gspch.h"
#include "GroundStationModule.h"

#include <memory>

#include "CRC.h"

void SerialPrint(Formatter& formatter, ModuleIDType from, LogLevelType level)
{
	SerialPrint(std::move(formatter), from, level);
}

//Implement global methods required by Module.h
void SerialPrint(Formatter&& formatter, ModuleIDType from, LogLevelType level)
{
	switch (level)
	{
		case LL_TRACE: HZ_TRACE("[{}]: {}", GetModuleIDName(from), formatter.c_str()); break;
		case LL_INFO: HZ_INFO("[{}]: {}", GetModuleIDName(from), formatter.c_str()); break;
		case LL_WARN: HZ_WARN("[{}]: {}", GetModuleIDName(from), formatter.c_str()); break;
		case LL_ERROR: HZ_ERROR("[{}]: {}", GetModuleIDName(from), formatter.c_str()); break;
	}
}

const char* GetParentModuleName()
{
	return "Ground Station";
}


std::unique_ptr<GroundStationModule> s_Module = nullptr;

SLICoreModule* GetDefaultCoreModule()
{
	if (s_Module.get() == nullptr)
	{
		s_Module.reset(new GroundStationModule());
	}
	return s_Module.get();
}


void GroundStationModule::Update()
{

}

void GroundStationModule::RoutePacket(PacketBuffer& packet)
{

}

extern "C"
{
	void Lights(int count)
	{
		HZ_INFO("Lights called. Blinking {} times", count);
	}
}