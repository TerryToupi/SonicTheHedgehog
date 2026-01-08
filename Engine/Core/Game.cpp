#include "Core/Game.h"

namespace core 
{
	void Game::SetRenderLoop(const Action& f)
	{
		m_Render = f;
	}

	void Game::SetAnimationLoop(const Action& f)
	{
		m_Anim = f;
	}

	void Game::SetInputLoop(const Action& f)
	{
		m_Input = f;
	}

	void Game::SetAILoop(const Action& f)
	{
		m_Ai = f;
	}

	void Game::SetPhysicsLoop(const Action& f)
	{
		m_Physics = f;
	}

	void Game::SetCollisionsCheckingLoop(const Action& f)
	{
		m_Collisions = f;
	}

	void Game::SetCommitDestructionLoop(const Action& f)
	{
		m_Destruct = f;
	}

	void Game::SetUserScriptingLoop(const Action& f)
	{
		m_User = f;
	}

	void Game::SetFinishingFunc(const Pred& f)
	{
		m_Done = f;
	}

	void Game::Render(void)
	{
		Invoke(m_Render);
	}

	void Game::ProgressAnimations(void)
	{
		Invoke(m_Anim);
	}

	void Game::Input(void)
	{
		Invoke(m_Input);
	}

	void Game::AI(void)
	{
		Invoke(m_Ai);
	}

	void Game::Physics(void)
	{
		Invoke(m_Physics);
	}

	void Game::CollisionChecking(void)
	{
		Invoke(m_Collisions);
	}

	void Game::CommitDestruction(void)
	{
		Invoke(m_Destruct);
	}

	void Game::UserScripting(void)
	{
		Invoke(m_User);
	}

	bool Game::IsFinished(void)
	{
		if (m_Done)
			return !m_Done();
		else
			return false;
	}

	void Game::SetOnPauseResume(const Action& f)
	{
		m_PauseResume = f;
	}

	void Game::Pause(TimeStamp t)
	{
		m_IsPaused = true;
		m_PauseTime = t;
		Invoke(m_PauseResume);
	}

	void Game::Resume(void)
	{
		m_IsPaused = false;
		Invoke(m_PauseResume);
		m_PauseTime = 0;
	}

	bool Game::IsPaused(void) const
	{
		return m_IsPaused;
	}

	TimeStamp Game::GetPauseTime(void) const
	{
		return m_PauseTime;
	}

	void Game::MainLoop(void)
	{
		while (!IsFinished())
			MainLoopIteration();
	}

	void Game::MainLoopIteration(void)
	{
		Render();
		Input();
		if (!IsPaused())
		{
			ProgressAnimations();
			AI();
			Physics();
			CollisionChecking();
			UserScripting();
			CommitDestruction();
		}
	}

	void Game::Invoke(const Action& f)
	{
		if (f) f();
	}
}