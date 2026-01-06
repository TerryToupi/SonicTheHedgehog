#include "Core/SystemClock.h"

namespace core
{
	auto SystemClock::Get(void) -> SystemClock&
	{
		return s_SystemClock;
	}

	core::SystemClock::Time SystemClock::milli_secs(void) const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_Clock.now().time_since_epoch()).count();
	}

	core::SystemClock::Time SystemClock::micro_secs(void) const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(m_Clock.now().time_since_epoch()).count();
	}

	core::SystemClock::Time SystemClock::nano_secs(void) const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(m_Clock.now().time_since_epoch()).count();
	}
}