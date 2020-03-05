#include "gspch.h"
#include "GroundStationModule.h"

#include <memory>

#include "CRC.h"


//Implement global methods required by Module.h
void SerialPrint(Formatter& formatter)
{
	formatter << '\n';

	HZ_INFO("Rocket: {}", formatter.c_str());
}


uint32_t CRC32Impl(const uint8_t* data, std::size_t bytes)
{
	return CRC::Calculate(data, bytes, CRC::CRC_32());

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

void GroundStationModule::RoutePacket(const PacketHeader& header, Buffer& packet)
{

}