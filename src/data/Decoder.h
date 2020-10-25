#pragma once

class SerialPort;

#include "Buffer.h"
#include "XBee.h"

class Decoder
{
public:
	static void Handle(RxDataResponse& response, SerialPort& port);

};

