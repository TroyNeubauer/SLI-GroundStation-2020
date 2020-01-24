#pragma once

#include <stdint.h>

#include <TUtil/vendor/str/Str.h>

enum class DataPointType
{
	//Types that are supported by all sources
	DISTANCE_TRAVELED, X, Y, Z, VELOCITY, VELOCITY_X, VELOCITY_Y, VELOCITY_Z, ACCELERATION, ACCELERATION_X, ACCELERATION_Y, ACCELERATION_Z, //Double
	STATUS, //String
	UPTIME, //Double (seconds)
};

enum class DataPointSource
{
	GPS, ALTIMETER_1, ALTIMETER_2, ACCELEROMETER, GROUND_STATION, STM_F103, STM_F205, OTHER
};



struct MKSUnit {
	//Each field here represents the exponent of that sub unit.
	int8_t Meters;
	int8_t Kilograms;
	int8_t Seconds;
};

MKSUnit GetUnitsFromType(DataPointType type);
void GetShortUnitName(MKSUnit units, char (&result)[16]);

struct Unit {
public:
	static const constexpr MKSUnit RATIO = { 0, 0, 0 };
	static const constexpr MKSUnit NONE = { 0, 0, 0 };

	static const constexpr MKSUnit METERS = { 1, 0, 0 };//m
	static const constexpr MKSUnit METERS2 = { 2, 0, 0 };//m^2 (area)
	static const constexpr MKSUnit METERS_PER_SECOND = { 1, 0, -1 };//m/s
	static const constexpr MKSUnit METERS_PER_SECOND2 = { 1, 0, -2 };//m/(s^2)

	static const constexpr MKSUnit KILOGRAMS = { 1, 0, 0 };

	static const constexpr MKSUnit SECONDS = { 0, 0, 1 };


};

struct DataPoint
{
	union {
		int64_t Int;
		double Double;
		Str16 String;
	} Data;

	DataPointType Type;
	DataPointSource Source;

	//The time this sample was recorded at
	uint64_t Seconds;//Seconds since Jan 1, 1970
	uint64_t NanoSeconds;//The nanoseconds within the second (0-999,999,999ns)

	inline MKSUnit Units() { return GetUnitsFromType(Type);  }
};
