#include "gspch.h"
#include "Decoder.h"

#include "Packet.h"
#include "util/SerialPort.h"
#include "main/MainLayer.h"
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
	static FILE* errors = nullptr;
	if (errors == nullptr)
	{
		errors = fopen("Errors.dat", "wb");
	}

	PacketHeader* header = buf.Header();
	if (header->Destination >= ModuleID::MAX_MODULE_ID)
	{
		InvalidPacketData data;
		data.InvalidValue.Offset = offsetof(PacketHeader, Destination);
		data.InvalidValue.InvalidValue = header->Destination;
		port.InvalidPacket(data, InvalidPacketError::INVALID_ADDRESS);
		return;
	}
	else if (header->From >= ModuleID::MAX_MODULE_ID)
	{
		InvalidPacketData data;
		data.InvalidValue.Offset = offsetof(PacketHeader, From);
		data.InvalidValue.InvalidValue = header->From;
		port.InvalidPacket(data, InvalidPacketError::INVALID_ADDRESS);
		return;
	}
	else if (header->Forwarder >= ModuleID::MAX_MODULE_ID)
	{
		InvalidPacketData data;
		data.InvalidValue.Offset = offsetof(PacketHeader, Forwarder);
		data.InvalidValue.InvalidValue = header->Forwarder;
		port.InvalidPacket(data, InvalidPacketError::INVALID_ADDRESS);
		return;
	}
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
			switch (header->From)
			{
				case ModuleID::GPS:
				{
					DataPacket_GPS* dataPacket = buf.Ptr<DataPacket_GPS>();
					port.Read(dataPacket, sizeof(DataPacket_GPS));
					if (dataPacket->NMEASentenceLength > 5 * MAX_NMEA_LENGTH)
					{
						InvalidPacketData data;
						data.InvalidValue.Offset = sizeof(PacketHeader) + offsetof(DataPacket_GPS, NMEASentenceLength);
						data.InvalidValue.InvalidValue = dataPacket->NMEASentenceLength;
						port.InvalidPacket(data, InvalidPacketError::INVALID_LENGTH);
						return;
					}
					char* nmeaSentenceRaw = buf.As<char>(dataPacket->NMEASentenceLength);
					port.Read(nmeaSentenceRaw, dataPacket->NMEASentenceLength);
					if (!VerifyCRC32(buf, port)) return;

					std::string nmeaSentence(nmeaSentenceRaw, dataPacket->NMEASentenceLength);
					port.GetMainLayer().HandleGPS(nmeaSentence);
					return;
				}
				default:
				{
					InvalidPacketData data;
					data.Information << "Unknown data packet for destination: " << GetModuleIDName(header->From);
					port.InvalidPacket(data, InvalidPacketError::UNKNOWN_PACKET);

					return;
				}
			}
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
			std::array<uint16_t, 8> errorChars;
			std::fill(errorChars.begin(), errorChars.end(), 0xFF00);
			fwrite(errorChars.data(), 1, sizeof(errorChars), errors);
			fwrite(buf.Begin(), 1, buf.Offset(), errors);
			fflush(errors);

			InvalidPacketData data;
			data.InvalidValue.Offset = offsetof(PacketHeader, Type);
			data.InvalidValue.InvalidValue = header->Type;
			port.InvalidPacket(data, InvalidPacketError::INVALID_PACKET_TYPE);
			return;
		}
	}
}
