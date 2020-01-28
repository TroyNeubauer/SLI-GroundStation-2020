#include <Hazel/Core/Core.h>
#include <TUtil/Timer.h>

#include "GSTime.h"


#ifdef HZ_PLATFORM_UNIX
	#include <time.h>
	
	GSTime GSTime::Now()
	{
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return { static_cast<std::uint64_t>(ts.tv_sec), static_cast<std::uint64_t>(ts.tv_nsec) };
	}

	void GSTime::InitClock()
	{
		//nop
	}



#elif defined(HZ_PLATFORM_WINDOWS)

	#include <chrono>

	std::uint64_t initalSeconds, initalMS;
	TUtil::Timer timer;

	GSTime GSTime::Now()
	{
		timer.Stop();
		uint64_t ns = timer.Nanos();
		constexpr uint64_t NS_PER_SECOND = 1000000000;
		return { initalSeconds + ns / NS_PER_SECOND, initalMS * 1000000 + ns % NS_PER_SECOND };
	}


	void GSTime::InitClock()
	{
		using namespace std::chrono;
		std::uint64_t initalMS = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		//Busy wait until the next millisecond starts
		std::uint64_t waitCount = 0;
		while (initalMS == duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count())
		{
			waitCount++;
		}
		timer.Start();
		milliseconds epochMS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		initalSeconds = duration_cast<seconds>(epochMS).count();
		initalMS = epochMS.count() % 1000;
		HZ_WARN("initalSeconds: {}, initalMS {}", initalSeconds, initalMS);

	}

	

#else
	#error bad platform

#endif

