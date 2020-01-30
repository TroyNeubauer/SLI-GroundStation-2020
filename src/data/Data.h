#pragma once

#include <vector>
#include <map>

#include <TUtil/StringUtils.h>

#include "GSTime.h"


enum class DataPointType
{
	//Types that are supported by all sources
	DISTANCE_TRAVELED, X, Y, Z, VELOCITY, VELOCITY_X, VELOCITY_Y, VELOCITY_Z, ACCELERATION, ACCELERATION_X, ACCELERATION_Y, ACCELERATION_Z, //Double
	STATUS, //String
	UPTIME, //Double (seconds)
};


enum class DataSource
{
	GPS, ALTIMETER_1, ALTIMETER_2, ACCELEROMETER, GROUND_STATION, STM_F103, STM_F205, OTHER
};


enum class DataAttachmentType
{
	VALUE, DERITIVE, DERITIVE2, INTEGRAL, INTEGRAL2
};


struct MKSUnit {
	//Each field here represents the exponent of that sub unit.
	std::int8_t Meters;
	std::int8_t Kilograms;
	std::int8_t Seconds;
	
	//Mutiplier for creating units like km, hours, etc
	std::int8_t Base = 10;
	std::int8_t Exponent = 0;
};

MKSUnit GetUnitsFromType(DataPointType type);
void GetShortUnitString(MKSUnit units, char (&result)[16]);

struct Unit {
public:
	static const constexpr MKSUnit RATIO = { 0, 0, 0 };
	static const constexpr MKSUnit NONE = { 0, 0, 0 };

	static const constexpr MKSUnit METERS = { 1, 0, 0 };//m
	static const constexpr MKSUnit METERS2 = { 2, 0, 0 };//m^2 (area)
	static const constexpr MKSUnit METERS_PER_SECOND = { 1, 0, -1 };//m/s
	static const constexpr MKSUnit METERS_PER_SECOND2 = { 1, 0, -2 };//m/(s^2)

	static const constexpr MKSUnit KILOGRAMS = { 0, 1, 0 };

	static const constexpr MKSUnit SECONDS = { 0, 0, 1 };

};


struct DataPoint
{
	union {
		std::int64_t Int;
		double Double;
		char String[16];
	};

	//The time this sample was recorded at
	GSTime Time;

public:
	inline DataPoint(uint64_t value, GSTime time = GSTime::Now()) : Int(value), Time(time) {}
	inline DataPoint(double value, GSTime time = GSTime::Now()) : Double(value), Time(time) {}
	inline DataPoint(const char* value, GSTime time = GSTime::Now()) : Time(time) { TUtil::StringUtils::Copy(String, sizeof(String), value); }

};

//Represents one specific graph
//EG. Accecelerometer velocity
struct DataAttachment
{
	DataPointType Type;
	std::vector<DataPoint> Points;
};


//Stores mutiple graphs associated with one sensor
//EG. GPS position + GPS velocity + GPS, acceleration
struct SensorDataEntry
{
	DataSource Source;

	std::map<DataAttachmentType, DataAttachment> Data;

public:
	SensorDataEntry(DataSource source, DataPointType type, double initalValue) : Source(source)
	{		
		Data[DataAttachmentType::VALUE].Type = type;
		Data[DataAttachmentType::VALUE].Points.emplace_back(initalValue);
	}

	//1 means first integral, -1 means first deritive, -2 means 2nd deritive etc.
	void AddAttachment(std::uint8_t integral)
	{
		DataAttachmentType type;
		switch (integral)
		{
			case  0: type = DataAttachmentType::VALUE; 		break;
			case  1: type = DataAttachmentType::INTEGRAL; 	break;
			case  2: type = DataAttachmentType::INTEGRAL2; 	break;
			case -1: type = DataAttachmentType::DERITIVE; 	break;
			case -2: type = DataAttachmentType::DERITIVE2; 	break;
		}
	}


	inline bool HasAttachment(DataAttachmentType attachment) const { return Data.find(attachment) != Data.end(); }

	inline MKSUnit Units() { return GetUnitsFromType(Data[DataAttachmentType::VALUE].Type);  }

};




