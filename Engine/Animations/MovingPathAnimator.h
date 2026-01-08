#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/MovingPathAnimation.h"

namespace anim
{
	class MovingPathAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currTime) override;
		unsigned GetCurrRep(void) const;
		unsigned GetCurrPath(void) const; 
		auto GetAnim(void) const -> const MovingPathAnimation&;

		void Start(MovingPathAnimation* a, TimeStamp t);

		MovingPathAnimator() = default;

	private:
		MovingPathAnimation* m_Anim = nullptr;
		unsigned m_CurrRep = 0;
		unsigned m_CurrPath = 0;
	};
}