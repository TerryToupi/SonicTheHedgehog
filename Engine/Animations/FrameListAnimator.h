#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/FrameListAnimation.h"

namespace anim
{
	class FrameListAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currtime) override;
		unsigned GetCurrFrame(void) const;
		unsigned GetCurrRep(void) const;
		auto GetAnim(void) const -> const FrameListAnimation&;

		void Start(FrameListAnimation* a, TimeStamp t, int _currFrame);

		FrameListAnimator() = default;

	private:
		FrameListAnimation* m_Anim = nullptr;
		unsigned m_CurrFrame = 0;
		unsigned m_CurrRep = 0;
		unsigned m_FrameIndex = 0;
	};
}
