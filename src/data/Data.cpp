#include "Data.h"

#include <Hazel/Core/Core.h>
#include <Hazel/Core/glm.h>

MKSUnit GetUnitsFromType(DataPointType type)
{
	switch (type)
	{
		case DataPointType::DISTANCE_TRAVELED:	return Unit::METERS;
		case DataPointType::X:					return Unit::METERS;
		case DataPointType::Y:					return Unit::METERS;
		case DataPointType::Z:					return Unit::METERS;

		case DataPointType::VELOCITY:			return Unit::METERS_PER_SECOND;
		case DataPointType::VELOCITY_X:			return Unit::METERS_PER_SECOND;
		case DataPointType::VELOCITY_Y:			return Unit::METERS_PER_SECOND;
		case DataPointType::VELOCITY_Z:			return Unit::METERS_PER_SECOND;

		case DataPointType::ACCELERATION:		return Unit::METERS_PER_SECOND2;
		case DataPointType::ACCELERATION_X:		return Unit::METERS_PER_SECOND2;
		case DataPointType::ACCELERATION_Y:		return Unit::METERS_PER_SECOND2;
		case DataPointType::ACCELERATION_Z:		return Unit::METERS_PER_SECOND2;

		case DataPointType::STATUS:				return Unit::NONE;
		case DataPointType::UPTIME:				return Unit::SECONDS;

		default: HZ_ASSERT(false, "Unknown DataPointType!"); return Unit::NONE;


	}
}

char DIGITS[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

void PrintSection(const char* unit, uint8_t exponent, char(&result)[16], int& offset)
{
	if (exponent == 0) return;

	while (*unit)
	{
		result[offset++] = unit[0];
		unit++;
	}
	if (exponent >= 2) result[offset++] = '^'; result[offset++] = DIGITS[exponent];
}

void GetShortUnitName(MKSUnit units, char(&result)[16])
{
	HZ_ASSERT(glm::abs(units.Meters) < 10, "Unit exponent out of range!");
	HZ_ASSERT(glm::abs(units.Kilograms) < 10, "Unit exponent out of range!");
	HZ_ASSERT(glm::abs(units.Seconds) < 10, "Unit exponent out of range!");

	int offset = 0;
	//Print numerator units first
	PrintSection("m",  units.Meters, result, offset);
	PrintSection("kg", units.Kilograms, result, offset);
	PrintSection("s",  units.Seconds, result, offset);

	//How many terms are in the denominator
	int denomCount = (units.Meters < 0) + (units.Kilograms < 0) + (units.Seconds < 0);
	if (denomCount > 0)
	{
		result[offset++] = '/';
		if (denomCount > 1) result[offset++] = '(';
		PrintSection("m",  -units.Meters, result, offset);
		PrintSection("kg", -units.Kilograms, result, offset);
		PrintSection("s",  -units.Seconds, result, offset);

		if (denomCount > 1) result[offset++] = ')';
	}

	result[offset++] = '\0';
	HZ_ASSERT(offset <= sizeof(result), "Unit string too large for buffer!");
}

