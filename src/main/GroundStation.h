#pragma once

#include <Hazel.h>

#include "data/Data.h"

class GroundStation : public Hazel::Application
{
public:
	GroundStation();

	virtual void Update(Hazel::Timestep ts) override;
	virtual void Render() override;

	~GroundStation();



private:
	GSTime m_StartTime;

};

Hazel::Application* HZCreateApplication(int argc, char** argv);
void HZGetApplicationInfo(Hazel::ApplicationInfo* info);

