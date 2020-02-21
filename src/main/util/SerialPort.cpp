#include "gspch.h"

#include "SerialPort.h"

#include <stdio.h>

#include <libusbp.hpp>
#include <Hazel.h>

std::unique_ptr<SerialPort> SerialPort::s_Instance;

const uint16_t radioVID = 0x0403;
const uint16_t radioPID = 0x6001;

#ifdef HZ_PLATFORM_WINDOWS

#include <ctype.h>

HANDLE handle = INVALID_HANDLE_VALUE;

void OpenSerialPort(const std::string& portName)
{
	std::string fullName = "\\\\.\\";
	fullName += portName;

	handle = CreateFileA(fullName.c_str(), GENERIC_READ | GENERIC_WRITE, // Read/Write Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);
}

bool GetWindowsPortName(std::string& portName)
{
	std::vector<int> ports;

	//Use QueryDosDevice to look for all devices of the form COMx. Since QueryDosDevice does
	//not consistently report the required size of buffer, lets start with a reasonable buffer size
	//of 4096 characters and go from there
	int nChars = 4096;
	bool bWantStop = false;
	while (nChars && !bWantStop)
	{
		std::vector<TCHAR> devices;
		devices.resize(nChars);

#pragma warning(suppress: 26446)
		const DWORD dwChars = QueryDosDevice(nullptr, &(devices[0]), nChars);
		if (dwChars == 0)
		{
			const DWORD dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				//Expand the buffer and  loop around again
				nChars *= 2;
			}
			else
				return false;
		}
		else
		{
			bWantStop = true;

			size_t i = 0;
#pragma warning(suppress: 6385 26446)
			while (devices[i] != _T('\0'))
			{
				//Get the current device name
#pragma warning(suppress: 26429 26446)
				LPCTSTR pszCurrentDevice = &(devices[i]);

				//If it looks like "COMX" then
				//add it to the array which will be returned
				const size_t nLen = _tcslen(pszCurrentDevice);
				if (nLen > 3)
				{
#pragma warning(suppress: 26481)
					if ((_tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0) && isdigit(pszCurrentDevice[3]))
					{
						//Work out the port number
#pragma warning(suppress: 26481)
						const int nPort = _ttoi(&pszCurrentDevice[3]);
#pragma warning(suppress: 26489)
						ports.push_back(nPort);
					}
				}

				//Go to next device name
				i += (nLen + 1);
			}
		}
	}

	if (ports.size() == 1)
	{
		portName = "COM" + std::to_string(ports[0]);
		HZ_INFO("XBee is on windows port {}", portName);
		return true;
	}
	else if (ports.size() > 1)
	{
		//FIXME. Prompt the usre which por to use
		HZ_WARN("Detected mutiple ports!");
		for (int id : ports)
		{
			HZ_WARN("\tCOM{}", id);
		}
	}

	return false;
}

bool IsPortOpen()
{
	return handle != INVALID_HANDLE_VALUE;
}

void CleanUpPort()
{
	CloseHandle(handle);
}


#elif HZ_PLATFORM_UNIX

int fd = -1;

void OpenSerialPort(const std::string& portName)
{

}

bool IsPortOpen()
{
	return fd != -1;
}

void CleanUpPort()
{
	CloseHandle(handle);
}



#else
	#error Unknown platform
#endif

bool GetPortName(std::string& portName)
{
	try
	{
		libusbp::device device = libusbp::find_device_with_vid_pid(radioVID, radioPID);
		if (device.pointer_get() == nullptr) return false;
		HZ_INFO("XBee USB Radio module detected");
#ifdef HZ_PLATFORM_WINDOWS
		return GetWindowsPortName(portName);
#else
		libusbp::serial_port port(device, 0, false);
		portName = port.get_name();
		return true;
#endif
	}
	catch (const std::exception & error)
	{
		HZ_ERROR("libusbp Error: {}", error.what());
		return false;
	}
}

void SerialPortLoop(SerialPort* port)
{
	while (port->IsRunning())
	{
		if (!IsPortOpen())
		{
			//No serial port yet
			std::string portName;
			if (GetPortName(portName))
			{
				//We found the radio's port name
				OpenSerialPort(portName);
			}
			else
			{
				//Wait for the operator to plug the radio in
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

		}
		else
		{
			//We are connected to the serial port


		}
	}
}


SerialPort::SerialPort() : m_Thread(SerialPortLoop, this)
{
	
}

void SerialPort::Close()
{
	m_Running = true;
	CleanUpPort();
	m_Thread.join();
}


SerialPort& SerialPort::GetPort()
{
	if (!s_Instance)
	{
		s_Instance.reset(new SerialPort());
	}
	return *s_Instance.get();
}

