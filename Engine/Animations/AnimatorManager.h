#pragma once

#include "Utils/Common.h"

#include <set>

namespace anim
{
	class Animator;

	class AnimatorManager final
	{
	public:
		void Register(Animator* a);
		void Cancel(Animator* a);

		void MarkAsRunning(Animator* a);
		void MarkAsSuspended(Animator* a);

		void Progress(TimeStamp currTime);
		void TimeShift(TimeStamp dt);

		static auto Get(void) -> AnimatorManager&;

	private:
		static AnimatorManager s_AnimatorManager;

		std::set<Animator*> m_Running, m_Suspended;

		AnimatorManager(void) = default;
		AnimatorManager(const AnimatorManager&) = delete;
		AnimatorManager(AnimatorManager&&) = delete;
	};
}