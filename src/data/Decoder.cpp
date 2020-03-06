#include "gspch.h"
#include "Decoder.h"

#include "Packet.h"
#include "util/SerialPort.h"
#include "Module.h"

bool VerifyCRC32(StackBuffer<4096>& buf, SerialPort& port)
{
	uint32_t crc = buf.CalculateCRC32();
	if (buf.Header()->CRC32 != crc)
	{
		InvalidPacketData data;
		data.InvalidChecksum.ExpectedCRC = crc;
		data.InvalidChecksum.ActualCRC = buf.Header()->CRC32;
		port.InvalidPacket(data, InvalidPacketError::INVALID_CHECKSUM);
		return false;
	}
	return true;
}

void Decoder::Handle(StackBuffer<4096>& buf, SerialPort& port)
{
	PacketHeader* header = buf.Header();
	switch (header->Type)
	{
		case PacketType::INIT:
		{

			return;
		}
		case PacketType::STATUS:
		{

			return;
		}
		case PacketType::DATA:
		{

			return;
		}
		case PacketType::MESSAGE:
		{
			MessagePacket* message = buf.Ptr<MessagePacket>();
			port.Read(message, sizeof(MessagePacket));

			if (message->MessageLength > MAX_MESSAGE_LENGTH)
			{
				InvalidPacketData data;
				data.InvalidValue.Offset = sizeof(PacketHeader) + offsetof(MessagePacket, MessageLength);
				data.InvalidValue.InvalidValue = message->MessageLength;
				port.InvalidPacket(data, InvalidPacketError::INVALID_LENGTH);
				return;
			}

			char* string = buf.As<char>(message->MessageLength);
			port.Read(string, message->MessageLength);
			if (!VerifyCRC32(buf, port)) return;

			const char* from = GetModuleIDName(header->From);
			std::string stdString(string, message->MessageLength);
			DefaultFormatter formatter;
			formatter << '[' << from << "]: " << stdString;
			SerialPrint(formatter, message->Level);

			return;
		}
		case PacketType::REQUEST:
		{

			return;
		}
		case PacketType::RESPONSE:
		{

			return;
		}
		default:
		{
			InvalidPacketData data;
			data.InvalidValue.Offset = offsetof(PacketHeader, Type);
			data.InvalidValue.InvalidValue = header->Type;
			port.InvalidPacket(data, InvalidPacketError::INVALID_PACKET_TYPE);
			return;
		}
	}
}
