#include "gspch.h"
#include "GroundStationModule.h"

#include "CRC.h"

void SerialPrint(Formatter& formatter)
{
	formatter << '\n';

	HZ_INFO("Rocket: {}", formatter.c_str());
}


uint32_t CRC32Impl(const uint8_t* data, std::size_t bytes)
{
	return CRC::Calculate(data, bytes, CRC::CRC_32());

}

