#include "Animations/AnimatorManager.h"
#include "Animations/Animator.h"
#include "Utils/Assert.h"

namespace anim
{
	AnimatorManager AnimatorManager::s_AnimatorManager;

	void AnimatorManager::Register(Animator* a)
	{
		ASSERT(a->HasFinished(), "FAILED. Animator has not finished yet");
		m_Suspended.insert(a);
	}

	void AnimatorManager::Cancel(Animator* a)
	{
		ASSERT(a->HasFinished(), "FAILED. Animator has not finished yet");
		m_Suspended.erase(a);
	}

	void AnimatorManager::MarkAsRunning(Animator* a)
	{
		ASSERT(!a->HasFinished(), "FAILED. Animator is allready running");
		m_Suspended.erase(a);
		m_Running.insert(a);
	}

	void AnimatorManager::MarkAsSuspended(Animator* a)
	{
		ASSERT(a->HasFinished(), "FAILED. Animator has not finished yet");
		m_Running.erase(a);
		m_Suspended.insert(a);
	}

	void AnimatorManager::Progress(TimeStamp currTime)
	{
		auto copied(m_Running);
		for (auto* a : copied)
			a->Progress(currTime);
	}

	void AnimatorManager::TimeShift(TimeStamp dt)
	{
		auto copied(m_Running);
		for (auto* a : copied)
			a->TimeShift(dt);
	}

	auto AnimatorManager::Get(void) -> AnimatorManager&
	{
		return s_AnimatorManager;
	}
}