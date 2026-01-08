#pragma once

#include "Utils/Common.h"

#include <functional>

namespace core
{
	class Game
	{
	public:
		using Action = std::function<void(void)>;
		using Pred = std::function<bool(void)>;

	public:
		void SetRenderLoop(const Action& f);
		void SetAnimationLoop(const Action& f);
		void SetInputLoop(const Action& f);
		void SetAILoop(const Action& f);
		void SetPhysicsLoop(const Action& f);
		void SetCollisionsCheckingLoop(const Action& f);
		void SetCommitDestructionLoop(const Action& f);
		void SetUserScriptingLoop(const Action& f);
		void SetFinishingFunc(const Pred& f);

		void Render(void);
		void ProgressAnimations(void);
		void Input(void);
		void AI(void);
		void Physics(void);
		void CollisionChecking(void);
		void CommitDestruction(void);
		void UserScripting(void);
		bool IsFinished(void);

		void SetOnPauseResume(const Action& f);
		void Pause(TimeStamp t);
		void Resume(void);

		bool	  IsPaused(void) const;
		TimeStamp GetPauseTime(void) const;

		void MainLoop(void);
		void MainLoopIteration(void);

		Game(void) = default;
		Game(const Game&) = delete;
		Game(Game&&) = delete;

	private:
		inline void Invoke(const Action& f);

	private:
		Action m_Render, m_Anim, m_Input, m_Ai, m_Physics, m_Collisions, m_User, m_Destruct; 
		Pred m_Done;

		Action m_PauseResume;
		bool m_IsPaused = false;
		TimeStamp m_PauseTime = 0;
	};
}