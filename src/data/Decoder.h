#pragma once

class PacketHeader;
class SerialPort;

class Decoder
{
public:
	static void HandlePacket(PacketHeader& header, SerialPort& port);
};

