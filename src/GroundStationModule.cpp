#include "gspch.h"
#include "GroundStationModule.h"

void SerialPrint(Formatter& formatter)
{
	formatter << '\n';

	HZ_INFO("Rocket: {}", formatter.c_str());
}
