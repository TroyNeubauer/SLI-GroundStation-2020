#include "gspch.h"

#include "SerialPort.h"
#include "main/MainLayer.h"
#include "Module.h"
#include "data/Decoder.h"

#include <stdio.h>

#include <libusbp.hpp>
#include <Hazel.h>

const uint16_t radioVID = 0x0403;
const uint16_t radioPID = 0x6001;

#if defined(HZ_PLATFORM_WINDOWS)

#include <ctype.h>

HANDLE handle = INVALID_HANDLE_VALUE;

void OpenSerialPort(const std::string& portName, SerialPort* port)
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

bool GetWindowsPortName(std::string& portName, SerialPort* port)
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
	port->GetMainLayer().Data.SerialPortStatus = MainData::SerialPortStatusEnum::CANNOT_FIND_VID;

	return false;
}

bool SerialPort::Read(void* dest, std::size_t bytes)
{
	return false;
}
bool SerialPort::Write(void* buf, std::size_t bytes)
{
	return false;
}

bool IsPortOpen(SerialPort* port)
{
	return handle != INVALID_HANDLE_VALUE;
}

void CleanUpPort()
{
	CloseHandle(handle);
}


#elif defined(HZ_PLATFORM_UNIX)

int fd = -1;

void OpenSerialPort(const std::string& portName, SerialPort* port)
{
	fd = open(portName.c_str(), O_RDWR);
}


bool SerialPort::Read(void* dest, std::size_t bytes)
{
	ssize_t bytesRead = read(fd, dest, bytes);
	if (bytesRead <= 0)
	{
		GetMainLayer().Data.SerialPortStatus = MainData::SerialPortStatusEnum::PORT_ERROR;
		close(fd);
		fd = -1;
		return false;
	}
	else
	{
		return true;
	}

}

bool SerialPort::Write(void* buf, std::size_t bytes)
{
	return false;
}

bool IsPortOpen(SerialPort* port)
{
	return fd != -1;
}

void CleanUpPort()
{
	
}



#else
	#error Unknown platform
#endif

bool GetPortName(std::string& portName, SerialPort* port)
{

	try
	{
		libusbp::device device = libusbp::find_device_with_vid_pid(radioVID, radioPID);
		if (device.pointer_get() == nullptr)
		{
			port->GetMainLayer().Data.SerialPortStatus = MainData::SerialPortStatusEnum::CANNOT_FIND_VID;
			return false;
		}
#ifdef HZ_PLATFORM_WINDOWS
		return GetWindowsPortName(portName);
#else
		HZ_INFO("About to open XBee on unix. Serial number: {}", device.get_serial_number());
		libusbp::serial_port port(device, 0, false);
		portName = port.get_name();
		HZ_INFO("Found XBee on unix port {}", portName);

		return true;
#endif
	}
	catch (const std::exception& error)
	{
		port->GetMainLayer().Data.SerialPortStatus = MainData::SerialPortStatusEnum::PORT_ERROR;

		HZ_ERROR("libusbp Error: {}", error.what());
		return false;
	}
}

void SerialPortLoop(SerialPort* port)
{

	HZ_INFO("Starting Serial Port thread");
	while (port->IsRunning())
	{
		if (!IsPortOpen(port))
		{
			//No serial port yet
			std::string portName;
			if (GetPortName(portName, port))
			{
				//We found the radio's port name
				OpenSerialPort(portName, port);
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
			PacketHeader header;
			port->Read(&header, sizeof(header));
			Decoder::HandlePacket(header, *port);
		}
	}
}


SerialPort::SerialPort(MainLayer& layer) : m_Thread(SerialPortLoop, this), m_Layer(layer)
{

}

void SerialPort::Close()
{
	m_Running = true;
	CleanUpPort();
	m_Thread.join();
}
