#pragma once

#include "Animations/Animation.h"

#include <string>

namespace anim
{
	class TickAnimation : public Animation
	{
	public:
		using Me = TickAnimation;

		unsigned GetDelay(void) const;
		Me& SetDelay(unsigned v);

		unsigned GetReps(void) const;
		Me& SetReps(unsigned n);

		bool IsForever(void) const;
		Me& SetForever(void);

		bool IsDiscrete(void) const;

		Animation* Clone(void) const override;

		TickAnimation(
			const std::string& id,
			unsigned d, unsigned r, bool discrete
		);

	protected:
		bool Inv(void) const;

	protected:
		unsigned m_Delay = 0;
		unsigned m_Reps = 1;
		bool m_IsDiscrete = true;
	};
}