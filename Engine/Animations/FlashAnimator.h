#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/FlashAnimation.h"

namespace anim
{
	class FlashShowAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currtime) override;
		unsigned GetShowDelay(void) const;
		unsigned GetCurrRep(void) const;
		auto GetAnim(void) const -> const FlashAnimation&;

		void Start(FlashAnimation* a, TimeStamp t);

		FlashShowAnimator() = default;

	private:
		FlashAnimation* m_Anim = nullptr;
		unsigned m_CurrRep = 0;
		unsigned m_ShowDelay = 0;
	};

	class FlashHideAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currtime) override;
		unsigned GetHideDelay(void) const;
		unsigned GetCurrRep(void) const;
		auto GetAnim(void) const -> const FlashAnimation&;

		void Start(FlashAnimation* a, TimeStamp t);

		FlashHideAnimator() = default;

	private:
		FlashAnimation* m_Anim = nullptr;
		unsigned m_CurrRep = 0;
		unsigned m_HideDelay = 0;
	};
}
