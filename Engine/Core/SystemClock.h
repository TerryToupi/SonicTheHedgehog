#pragma once

#include "Utils/Common.h"

#include <chrono>

namespace core
{
	class SystemClock final
	{
	public:
		static auto Get(void) -> SystemClock&;
		Time milli_secs(void) const;
		Time micro_secs(void) const;
		Time nano_secs(void) const;

		void	  SetCurrTime();
		TimeStamp GetCurrTime() const;
		void      ClearCurrTime();

	private:
		static SystemClock s_SystemClock;

		std::chrono::high_resolution_clock m_Clock;
		TimeStamp						   m_CurrTime = 0;
	};
}