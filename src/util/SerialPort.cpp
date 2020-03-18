#include "gspch.h"

#include "SerialPort.h"
#include "main/MainLayer.h"
#include "Packet.h"
#include "data/Decoder.h"

#include <cstdio>
#include <chrono>
#include <thread>

#include <libusbp.hpp>
#include <Hazel.h>
#include <XBee.h>

const uint16_t radioVID = 0x0403;
const uint16_t radioPID = 0x6001;

const uint32_t BAUD_RATE = 115200;

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

	if (handle == INVALID_HANDLE_VALUE)
	{
		HZ_ERROR("Failed to open serial port!");
		return;
	}
	HZ_INFO("Opened port {}", portName);

	DCB dcbConfig;

	if (GetCommState(handle, &dcbConfig))
	{
		dcbConfig.BaudRate = BAUD_RATE;
		dcbConfig.ByteSize = 8;
		dcbConfig.Parity = NOPARITY;
		dcbConfig.StopBits = ONESTOPBIT;
		dcbConfig.fBinary = TRUE;
		dcbConfig.fParity = TRUE;
	}

	else
	{
		HZ_ERROR("GetCommState Failed!");
		return;
	}
	if (!SetCommState(handle, &dcbConfig))
	{
		HZ_ERROR("SetCommState Failed!");
		return;
	}
	
	//Disable timeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(handle, &timeouts))
	{
		HZ_ERROR("SetCommTimeouts Failed!");
		return;
	}
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
		std::vector<std::string> avilablePorts;
		for (int id : ports)
		{
			DefaultFormatter formatter;
			formatter << "COM" << id;
			avilablePorts.emplace_back(formatter.Data(), formatter.Size());
		}
		std::promise<std::string> promise;
		port->GetMainLayer().OpenDialog(avilablePorts, &promise);
		portName = promise.get_future().get();
		HZ_INFO("User selected port {}", portName);
		return true;
	}
	port->GetMainLayer().Data.SerialPortStatus = MainData::SerialPortStatusEnum::CANNOT_FIND_VID;

	return false;
}

bool SerialPort::Read(void* dest, std::size_t bytes)
{
	if (handle == INVALID_HANDLE_VALUE) return false;

	DWORD bytesRead;
	if (!ReadFile(handle, dest, bytes, &bytesRead, nullptr) || (bytesRead != bytes))
	{
		GetMainLayer().Data.SerialPortStatus = MainData::SerialPortStatusEnum::PORT_ERROR;
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
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
	return handle != INVALID_HANDLE_VALUE;
}

void CleanUpPort()
{
	CloseHandle(handle);
	handle = INVALID_HANDLE_VALUE;
}

void DiscardRXBuffer()
{
	PurgeComm(handle, PURGE_RXCLEAR);
	HZ_INFO("Clearing serial port buffer to clense corrupt packet");
}


#elif defined(HZ_PLATFORM_UNIX)

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

int fd = -1;

void OpenSerialPort(const std::string& portName, SerialPort* port)
{
	fd = open(portName.c_str(), O_RDWR| O_NOCTTY | O_NDELAY);
		
	struct termios serialPortSettings;	/* Create the structure                          */

	
	tcgetattr(fd, &serialPortSettings);	/* Get the current attributes of the Serial port */

	/* Setting the Baud rate */
	cfsetispeed(&serialPortSettings, B1152000); /* Set Read  Speed as 9600                       */
	cfsetospeed(&serialPortSettings, B1152000); /* Set Write Speed as 9600                       */

	/* 8N1 Mode */
	serialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
	serialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
	serialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
	serialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
	
	serialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
	serialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
	
	
	serialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
	serialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

	serialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
	
	/* Setting Time outs */
	serialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
	serialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */


	if((tcsetattr(fd, TCSANOW, &serialPortSettings)) != 0) /* Set the attributes to the termios structure*/
		HZ_ERROR("ERROR ! in Setting attributes");
	else
		HZ_INFO("BaudRate = 115200, StopBits = 1, Parity = none");
		
		/*------------------------------- Read data from serial port -----------------------------*/

	tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */

}

void DiscardRXBuffer()
{
	tcflush(fd, TCIFLUSH);
	HZ_INFO("Clearing serial port buffer to clense corrupt packet");
}


bool SerialPort::Read(void* dest, std::size_t bytes)
{
	if (fd == -1) return false;

	ssize_t bytesRead = read(fd, dest, bytes);
	if (bytesRead != bytes)
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
	if (fd == -1) return false;

	ssize_t bytesWritten = write(fd, buf, bytes);
	if (bytesWritten != bytes)
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

bool IsPortOpen(SerialPort* port)
{
	return fd != -1;
}

void CleanUpPort()
{
	close(fd);
	fd = -1;
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
		return GetWindowsPortName(portName, port);
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

struct SerialPortData
{
	MainLayer* Layer;
	XBeeWithCallbacks* Xbee;
	SerialPort* Port;
};

void PrintResponse(XBeeResponse& res, uintptr_t d)
{
	SerialPortData* data = reinterpret_cast<SerialPortData*>(d);
	HZ_WARN("Unknown responce ID: {}, Length: {}", res.getApiId(), res.getPacketLength());
}

void ZBRxResponseCallback(ZBRxResponse& res, uintptr_t d)
{
	HZ_TRACE("Got packet. {} bytes", res.getDataLength());
	SerialPortData* data = reinterpret_cast<SerialPortData*>(d);
	Decoder::Handle(res, *data->Port);
}

void ZBExplicitRxResponseCallback(ZBExplicitRxResponse& res, uintptr_t d)
{
	HZ_TRACE("Got packet. {} bytes", res.getDataLength());
	SerialPortData* data = reinterpret_cast<SerialPortData*>(d);
	Decoder::Handle(res, *data->Port);
}

void SerialPortLoop(SerialPort* port)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	HZ_INFO("Starting Serial Port thread");

	XBeeWithCallbacks xbee;
	SerialPortData data;
	data.Layer = &port->GetMainLayer();
	data.Port = port;
	data.Xbee = &xbee;

	xbee.onOtherResponse(PrintResponse, reinterpret_cast<uintptr_t>(&data));
	xbee.onResponse(nullptr, 0);
	xbee.onAtCommandResponse(nullptr, 0);
	xbee.onModemStatusResponse(nullptr, 0);
	xbee.onPacketError(nullptr, 0);
	xbee.onRemoteAtCommandResponse(nullptr, 0);
	xbee.onRx16IoSampleResponse(nullptr, 0);
	xbee.onRx16Response(nullptr, 0);
	xbee.onRx64IoSampleResponse(nullptr, 0);
	xbee.onRx64Response(nullptr, 0);
	xbee.onTxStatusResponse(nullptr, 0);
	xbee.onZBRxIoSampleResponse(nullptr, 0);
	xbee.onZBTxStatusResponse(nullptr, 0);

	xbee.onZBRxResponse(ZBRxResponseCallback, reinterpret_cast<uintptr_t>(&data));
	xbee.onZBExplicitRxResponse(ZBExplicitRxResponseCallback, reinterpret_cast<uintptr_t>(&data));

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
			xbee.loop();

		}
	}
	HZ_INFO("Serial Port thread exiting");
}

void SerialPort::InvalidPacket(InvalidPacketData& data, InvalidPacketError error)
{
	DiscardRXBuffer();
	if (error == InvalidPacketError::INVALID_CHECKSUM)
	{
		HZ_WARN("Corrupt packet recieved! Expected checksum 0x{:x} but got 0x{:x}", data.InvalidChecksum.ExpectedCRC, data.InvalidChecksum.ActualCRC);
	}
	else if (error == InvalidPacketError::UNKNOWN_PACKET)
	{
		HZ_WARN("Unknown packet: {}", data.Information.c_str());
	}
	else
	{
		const char* info;
		switch (error)
		{
			case InvalidPacketError::INVALID_MAGIC: info = "Invalid Magic"; break;
			case InvalidPacketError::INVALID_PACKET_TYPE: info = "Invalid Packet Type"; break;
			case InvalidPacketError::INVALID_LENGTH: info = "Invalid Length, too large";
			default: info = "Unspecified problem"; break;
		}
		HZ_WARN("Invalid recieved! ({}) Valud is wrong at offset {}, value is: 0x{:x}", info, data.InvalidValue.Offset, data.InvalidValue.InvalidValue);

	}

}

bool s_HasBufferedByte = false;
uint8_t s_Buffer;

bool SerialPort::available()
{
	if (!s_HasBufferedByte)
	{
		Read(&s_Buffer, sizeof(s_Buffer));
		s_HasBufferedByte = true;
	}
	return true;
}

uint8_t SerialPort::read()
{
	if (s_HasBufferedByte)
	{
		s_HasBufferedByte = false;
		return s_Buffer;
	}
	uint8_t buf;
	Read(&buf, sizeof(buf));
	return buf;
}

void SerialPort::write(uint8_t value)
{
	Write(&value, sizeof(value));
}

Stream* XBeeSerial = nullptr;


SerialPort::SerialPort(MainLayer& layer) : m_Thread(SerialPortLoop, this), m_Layer(layer)
{
	HZ_ASSERT(!XBeeSerial, "XBeeSerial already exists! Cannot create mutiple serial ports!");
	XBeeSerial = this;
}

void SerialPort::Close()
{
	m_Running = false;
	CleanUpPort();
	m_Thread.join();
}

