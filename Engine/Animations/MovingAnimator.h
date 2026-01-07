#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/MovingAnimation.h"

namespace anim
{
	class MovingAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currTime) override;
		virtual void ProgressContinuous(TimeStamp currTime);
		auto GetAnim(void) const -> const MovingAnimation&;

		void Start(MovingAnimation* a, TimeStamp t);

		MovingAnimator(void) = default;

	protected:
		MovingAnimation* m_Anim = nullptr;
		unsigned m_CurrRep = 0;
	};
}