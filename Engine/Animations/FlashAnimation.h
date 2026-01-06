#pragma once

#include "Animations/Animation.h"

namespace anim
{
	class FlashAnimation : public Animation
	{
	public:
		using Me = FlashAnimation;

		Me& SetReps(unsigned n);
		unsigned GetReps(void) const;

		Me& SetHideDelay(unsigned n);
		unsigned GetHideDelay(void) const;

		Me& SetShowDelay(unsigned n);
		unsigned GetShowDelay(void) const;
	
		Animation* Clone(void) const override;

		FlashAnimation(
			const std::string& id,
			unsigned n, unsigned show, unsigned hide
		);

	private:
		unsigned m_Repetitions = 0;
		unsigned m_HideDelay = 0;
		unsigned m_ShowDelay = 0;
	};
}
