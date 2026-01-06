#include "Core/SystemClock.h"

namespace core
{
	auto SystemClock::Get(void) -> SystemClock&
	{
		return s_SystemClock;
	}

	Time SystemClock::milli_secs(void) const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_Clock.now().time_since_epoch()).count();
	}

	Time SystemClock::micro_secs(void) const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(m_Clock.now().time_since_epoch()).count();
	}

	Time SystemClock::nano_secs(void) const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(m_Clock.now().time_since_epoch()).count();
	}
}