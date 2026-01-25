#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/ScrollAnimation.h"

namespace anim
{
	class ScrollAnimator : public Animator
	{
		virtual void Progress(Time currtime) override;

		unsigned GetCurrRep(void) const;
		unsigned GetCurrScroll(void) const;

		void Start(ScrollAnimation* a, Time t);

		ScrollAnimator();

	protected:
		ScrollAnimation* m_Anim = nullptr;
		unsigned m_CurrRep = 0;
		unsigned m_CurrScroll = 0;
	};
}