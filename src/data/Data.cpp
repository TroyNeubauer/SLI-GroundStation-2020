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

		default: HZ_ASSERT(false, "Unknown DataPointType!");


	}
}

char DIGITS[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

void GetShortUnitName(MKSUnit units, char(&result)[16])
{
	HZ_ASSERT(glm::abs(units.Meters) < 10, "Unit exponent out of range!");
	HZ_ASSERT(glm::abs(units.Kilograms) < 10, "Unit exponent out of range!");
	HZ_ASSERT(glm::abs(units.Seconds) < 10, "Unit exponent out of range!");

	int offset = 0;
	//Print numerator units first
	if (units.Meters > 0)
	{
		result[offset++] = 'm';
		if (units.Meters >= 2) result[offset++] = '^'; result[offset++] = DIGITS[units.Meters];
	}
	if (units.Kilograms > 0)
	{
		result[offset++] = 'k';
		result[offset++] = 'g';
		if (units.Kilograms >= 2) result[offset++] = '^'; result[offset++] = DIGITS[units.Kilograms];
	}
	if (units.Seconds > 0)
	{
		result[offset++] = 's';
		if (units.Meters >= 2) result[offset++] = '^'; result[offset++] = DIGITS[units.Seconds];
	}

	//How many terms are in the denominator
	int denomCount = (units.Meters < 0) + (units.Kilograms < 0) + (units.Seconds < 0);
	if (denomCount > 0)
	{
		result[offset++] = '/';
		if (denomCount > 1) result[offset++] = '(';
		if (units.Meters < 0)
		{
			result[offset++] = 'm';
			if (units.Meters <= -2) result[offset++] = '^'; result[offset++] = DIGITS[-units.Meters];
		}
		if (units.Kilograms < 0)
		{
			result[offset++] = 'k';
			result[offset++] = 'g';
			if (units.Kilograms <= -2) result[offset++] = '^'; result[offset++] = DIGITS[-units.Kilograms];
		}
		if (units.Seconds < 0)
		{
			result[offset++] = 's';
			if (units.Meters <= -2) result[offset++] = '^'; result[offset++] = DIGITS[-units.Seconds];
		}


		if (denomCount > 1) result[offset++] = ')';
	}
	HZ_ASSERT(offset < sizeof(result), "Unit string too large for buffer!");
}

