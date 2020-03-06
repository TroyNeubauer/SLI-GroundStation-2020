#pragma once

#include <memory>
#include <thread>

class MainLayer;

enum class InvalidPacketError
{
	INVALID_CHECKSUM, INVALID_PACKET_TYPE, INVALID_MAGIC, INVALID_LENGTH
};

struct InvalidPacketData
{
	union
	{
		struct
		{
			uint32_t ExpectedCRC, ActualCRC;
		} InvalidChecksum;

		struct
		{
			uint32_t Offset;
			uint32_t InvalidValue;
		} InvalidValue;
	};
};

class SerialPort
{
public:
	SerialPort(MainLayer& layer);
	inline bool IsRunning() const { return m_Running; }
	void Close();
	MainLayer& GetMainLayer() { return m_Layer; }

	bool Read(void* dest, std::size_t bytes);
	bool Write(void* src, std::size_t bytes);

	void InvalidPacket(const InvalidPacketData& data, InvalidPacketError error);

private:
	bool m_Running = true;
	std::thread m_Thread;
	MainLayer& m_Layer;

};
