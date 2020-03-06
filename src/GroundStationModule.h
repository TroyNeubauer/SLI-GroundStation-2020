#pragma once

#include "Module.h"

class GroundStationModule : public SLICoreModule
{
public:
	GroundStationModule() : SLICoreModule(ModuleID::GROUND_STATION) {}

	virtual void Update();

	virtual ~GroundStationModule() {}

private:
	virtual void RoutePacket(const PacketHeader& header, Buffer& packet);

};
