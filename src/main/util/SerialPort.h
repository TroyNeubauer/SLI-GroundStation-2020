#pragma once

#include <memory>
#include <thread>

class MainLayer;

class SerialPort
{
public:
	SerialPort(MainLayer& layer);
	inline bool IsRunning() const { return m_Running; }
	void Close();
	MainLayer& GetMainLayer() { return m_Layer; }

	bool Read(void* dest, std::size_t bytes);
	bool Write(void* src, std::size_t bytes);

private:
	bool m_Running = true;
	std::thread m_Thread;
	MainLayer& m_Layer;

};
