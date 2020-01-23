#pragma once

#include <Hazel.h>


class GroundStation : public Hazel::Application
{
public:
	GroundStation();

	virtual void Update(Hazel::Timestep ts) override;
	virtual void Render() override;

	~GroundStation();



private:


};

Hazel::Application* HZCreateApplication(int argc, char** argv);
void HZGetApplicationInfo(Hazel::ApplicationInfo* info);

