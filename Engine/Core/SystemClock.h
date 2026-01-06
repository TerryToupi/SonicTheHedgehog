#pragma once

#include <chrono>

namespace core
{
	class SystemClock final
	{
	public:
		using Time = uint64_t;

		static auto Get(void) -> SystemClock&;
		Time milli_secs(void) const;
		Time micro_secs(void) const;
		Time nano_secs(void) const;

	private:
		static SystemClock s_SystemClock;

		std::chrono::high_resolution_clock m_Clock;
	};
}