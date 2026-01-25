#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/TickAnimation.h"

namespace anim
{
	class TickAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currTime) override;

		unsigned GetCurrRep(void) const;
		unsigned GetElapsedTime(void) const;
		float GetElapsedTimeNormalised(void) const;
		auto GetAnim(void) const -> const TickAnimation&;

		void Start(TickAnimation* a, TimeStamp t);

		TickAnimator(void);

	protected:
		TickAnimation* m_Anim = nullptr;
		unsigned m_CurrRep = 0;
		unsigned m_ElapsedTime = 0;
	};
}