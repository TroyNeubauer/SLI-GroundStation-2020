#pragma once

#include <Hazel.h>


class MainLayer : public Hazel::Layer
{
	MainLayer() : Hazel::Layer("MainLayer") {}

	virtual void OnAttach();
	virtual void OnDetach();
	virtual void OnUpdate(Hazel::Timestep ts);
	virtual void OnEvent(Hazel::Event* event);
	virtual void Render();
	virtual void OnImGuiRender();
	virtual ~MainLayer();


};
