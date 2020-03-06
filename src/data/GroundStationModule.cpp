#include "gspch.h"
#include "GroundStationModule.h"

#include <memory>

#include "CRC.h"

void SerialPrint(Formatter& formatter, LogLevelType level)
{
	SerialPrint(std::move(formatter), level);
}

//Implement global methods required by Module.h
void SerialPrint(Formatter&& formatter, LogLevelType level)
{
	switch (level)
	{
		case LL_TRACE: HZ_TRACE("{}", formatter.c_str()); break;
		case LL_INFO: HZ_INFO("{}", formatter.c_str()); break;
		case LL_WARN: HZ_WARN("{}", formatter.c_str()); break;
		case LL_ERROR: HZ_ERROR("{}", formatter.c_str()); break;
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