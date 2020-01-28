
#include <cstdint>

struct GSTime
{
	std::uint64_t Seconds;//Seconds since Jan 1, 1970
	std::uint64_t NanoSeconds;//The nanoseconds within the second (0-999,999,999ns)

	static GSTime Now();
	static void InitClock();

};



