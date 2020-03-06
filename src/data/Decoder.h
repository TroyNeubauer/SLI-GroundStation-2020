#pragma once

class SerialPort;

#include "Buffer.h"

class Decoder
{
public:
	static void Handle(StackBuffer<4096>& buf, SerialPort& port);

};

