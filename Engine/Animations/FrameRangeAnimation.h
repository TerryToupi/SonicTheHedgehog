#pragma once

#include "Animations/MovingAnimation.h"

namespace anim
{
	class FrameRangeAnimation : public MovingAnimation
	{
	public:
		using Me = FrameRangeAnimation;

		unsigned GetStartFrame(void) const;
		Me& SetStartFrame(unsigned v);

		unsigned GetEndFrame(void) const;
		Me& SetEndFrame(unsigned v);

		Animation* Clone(void) const override;

		FrameRangeAnimation(
			const std::string& id,
			unsigned s, unsigned e,
			unsigned r, int dx, int dy, int d
		);

	protected:
		unsigned m_Start = 0;
		unsigned m_End = 0;
	};
}
