#include "Core/SystemClock.h"

namespace core
{ 
	SystemClock SystemClock::s_SystemClock;

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

	void SystemClock::SetCurrTime()
	{
		m_CurrTime = milli_secs();
	}

	TimeStamp SystemClock::GetCurrTime() const
	{
		return m_CurrTime;
	}

	void SystemClock::ClearCurrTime()
	{
		m_CurrTime = 0;
	}
}