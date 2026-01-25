#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/FrameRangeAnimation.h"

namespace anim
{
	class FrameRangeAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currTime) override;

		unsigned GetCurrFrame(void) const;
		unsigned GetPrevFrame(void) const;
		unsigned GetCurrRep(void) const;
		auto GetAnim(void) -> const FrameRangeAnimation&;

		bool HasJustStarted(void) const;

		void Start(FrameRangeAnimation* a, TimeStamp t);

		FrameRangeAnimator(void);

	protected:
		FrameRangeAnimation* m_Anim = nullptr;
		unsigned m_CurrFrame = 0, m_PrevFrame = 0;
		unsigned m_CurrRep = 0;
	};
}
