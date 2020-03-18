#include "gspch.h"
#include "Decoder.h"

#include "Packet.h"
#include "util/SerialPort.h"
#include "main/MainLayer.h"
#include "Module.h"

void Decoder::Handle(RxDataResponse& response, SerialPort& port)
{
	PacketBuffer buf(response.getData(), response.getDataLength());
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
			StatusPacket* status = buf.Ptr<StatusPacket>();
			if (status->Status == StatusValue::MODULE_ERROR)
			{
				ErrorStatusPacket* errorStatus = buf.Ptr< ErrorStatusPacket>();
				HZ_ERROR("Error in {}, Code: {}, while doing: {}", GetModuleIDName(header->From), errorStatus->ErrorCode, errorStatus->Operation);
			}
			else
			{
				HZ_INFO("[{}]: Status: {}", GetModuleIDName(header->From), status->Status);
			}

			return;
		}
		case PacketType::DATA:
		{
			switch (header->From)
			{
				case ModuleID::GPS:
				{
					DataPacket_GPS* dataPacket = buf.Ptr<DataPacket_GPS>();
					if (dataPacket->NMEASentenceLength > MAX_PACKET_SIZE)
					{
						InvalidPacketData data;
						data.InvalidValue.Offset = sizeof(PacketHeader) + offsetof(DataPacket_GPS, NMEASentenceLength);
						data.InvalidValue.InvalidValue = dataPacket->NMEASentenceLength;
						port.InvalidPacket(data, InvalidPacketError::INVALID_LENGTH);
						return;
					}
					char* nmeaSentenceRaw = buf.As<char>(dataPacket->NMEASentenceLength);

					std::string nmeaSentence(nmeaSentenceRaw, dataPacket->NMEASentenceLength);
					port.GetMainLayer().HandleGPS(nmeaSentence);
					return;
				}
				case ModuleID::STM32F103:
				{
					DataPacket_STMF103* data = buf.Ptr<DataPacket_STMF103>();
					HZ_INFO("HZ: {}", data->UpdateLoopHZ);
					return;
				}
				default:
				{
					HZ_ASSERT(false, "Packet handler needs implementation");

					return;
				}
			}
		}
		case PacketType::MESSAGE:
		{
			MessagePacket* message = buf.Ptr<MessagePacket>();

			if (message->MessageLength > MAX_MESSAGE_LENGTH || message->Level >= MAX_MESSAGE_LEVEL)
			{
				InvalidPacketData data;
				data.InvalidValue.Offset = sizeof(PacketHeader) + offsetof(MessagePacket, MessageLength);
				data.InvalidValue.InvalidValue = message->MessageLength;
				port.InvalidPacket(data, InvalidPacketError::INVALID_LENGTH);
				return;
			}

			char* string = buf.As<char>(message->MessageLength);

			const char* from = GetModuleIDName(header->From);
			std::string stdString(string, message->MessageLength);
			DefaultFormatter formatter;
			formatter << '[' << from << "]: " << stdString;
			SerialPrint(formatter, header->From, message->Level);

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
