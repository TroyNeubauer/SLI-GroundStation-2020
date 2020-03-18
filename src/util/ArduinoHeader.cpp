#include "gspch.h"
#include "ArduinoHeader.h"

TUtil::Timer s_Timer;
bool s_Init = false;

uint32_t millis()
{
	if (!s_Init)
	{
		s_Init = true;
		s_Timer.Start();
	}
	return static_cast<uint32_t>(s_Timer.Stop().Millis());
}
