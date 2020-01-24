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

void PrintSection(const char* unit, int8_t exponent, char(&result)[16], int& offset, int& printCount)
{
	if (exponent == 0) return;
	HZ_ASSERT(glm::abs(exponent) < 10, "Unit exponent out of range!");
	if (printCount > 0)
	{
		result[offset++] = ' ';
	}
	while (*unit)
	{
		result[offset++] = unit[0];
		unit++;
	}
	//For all exponents other than 1 we need to print the exponent
	if (exponent != 1)
	{
		result[offset++] = '^';
		if (exponent >= 0)
		{
			result[offset++] = DIGITS[exponent];
		}
		else
		{
			result[offset++] = '-';
			result[offset++] = DIGITS[-exponent];
		}
		
	}
	printCount++;
}

void GetShortUnitString(MKSUnit units, char(&result)[16])
{
	int offset = 0;
	int printCount = 0;
	//Print numerator units first
	if (units.Meters > 0) PrintSection("m",  units.Meters, result, offset, printCount);
	if (units.Kilograms > 0) PrintSection("kg", units.Kilograms, result, offset, printCount);
	if (units.Seconds > 0) PrintSection("s",  units.Seconds, result, offset, printCount);

	printCount = 0;
	//How many terms are in the numerator & denominator
	int neumCount = (units.Meters > 0) + (units.Kilograms > 0) + (units.Seconds > 0);
	int denomCount = (units.Meters < 0) + (units.Kilograms < 0) + (units.Seconds < 0);
	if (denomCount > 0)
	{
		if (neumCount == 0)
		{
			//Use negitive exponents
			if (units.Meters < 0) PrintSection("m",  units.Meters, result, offset, printCount);
			if (units.Kilograms < 0) PrintSection("kg", units.Kilograms, result, offset, printCount);
			if (units.Seconds < 0) PrintSection("s",  units.Seconds, result, offset, printCount);

		}
		else
		{
			//There are sub-units in the numerator so dump the negitive exponents in the denominator
			result[offset++] = '/';
			if (denomCount > 1) result[offset++] = '(';
			
			if (units.Meters < 0) PrintSection("m",  -units.Meters, result, offset, printCount);
			if (units.Kilograms < 0) PrintSection("kg", -units.Kilograms, result, offset, printCount);
			if (units.Seconds < 0) PrintSection("s",  -units.Seconds, result, offset, printCount);

			if (denomCount > 1) result[offset++] = ')';	
		}
	}

	result[offset++] = '\0';
	HZ_ASSERT(offset <= sizeof(result), "Unit string too large for buffer!");
}

