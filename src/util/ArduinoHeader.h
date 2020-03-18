#pragma once

#include <cstdint>

class Stream
{
public:
	virtual bool available() = 0;
	virtual uint8_t read() = 0;
	virtual void write(uint8_t value) = 0;
};

uint32_t millis();

extern Stream* XBeeSerial;
