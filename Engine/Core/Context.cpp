#include "Core/Context.h"
#include "Core/SystemClock.h"

namespace core
{
	void Context::Run(void)
	{
		SystemClock::Get().SetCurrTime();
		m_Game.MainLoop();
	}

	void Context::RunIteration(void)
	{
		m_Game.MainLoopIteration();
	}

	const Game& Context::GetGame(void) const
	{
		return m_Game;
	}

	core::Game& Context::GetGame(void)
	{
		return m_Game;
	}

	void Context::Start(void)
	{
		SystemClock::Get().ClearCurrTime();
		Initialize();
		Load();
		Run();
		Clear();
		SystemClock::Get().ClearCurrTime();
	}
}
