#pragma once

#include <memory>
#include <thread>

class SerialPort
{
public:
	SerialPort();
	inline bool IsRunning() const { return m_Running; }
	void Close();

	//Nop. Ensures an instance exists;
	inline void Init() const {}

private:
	bool m_Running = true;
	std::thread m_Thread;

public:
	static SerialPort& GetPort();

private:
	static std::unique_ptr<SerialPort> s_Instance;
};
