#pragma once

#include <Hazel.h>

#include "Module.h"

class SerialPort;

struct MainData
{
	enum class SerialPortStatusEnum
	{
		OK, CANNOT_FIND_VID, MUTIPLE_DETECTED, PORT_OPEN, PORT_ERROR
	};

	SerialPortStatusEnum SerialPortStatus = SerialPortStatusEnum::OK;

};

class MainLayer : public Hazel::Layer
{
public:
	MainLayer() : Hazel::Layer("MainLayer") {}

	virtual void OnAttach();
	virtual void OnDetach();
	virtual void OnUpdate(Hazel::Timestep ts);
	virtual void OnEvent(Hazel::Event* event);
	virtual void Render();
	virtual void OnImGuiRender();
	virtual ~MainLayer();

	void HandleGPS(const std::string& sentence);

	MainData Data;

private:
	SerialPort* m_Port = nullptr;
	SLICoreModule* m_GroundStation;


};
