#pragma once

#include "Animations/Animation.h"

#include <string>

namespace anim
{
	class MovingAnimation : public Animation
	{
	public:
		using Me = MovingAnimation;

		int GetDx(void) const;
		Me& SetDx(int v);

		int GetDy(void) const;
		Me& SetDy(int v);

		unsigned GetDelay(void) const;
		Me& SetDelay(unsigned v);

		unsigned GetReps(void) const;
		Me& SetReps(unsigned n);

		bool IsForever(void) const;
		Me& SetForever(void);

		bool IsAnalogue(void) const;

		Animation* Clone(void) const override;

		MovingAnimation(
			const std::string& id, unsigned reps, int dx, int dy, unsigned delay
		);

		MovingAnimation(
			int dx, int dy
		);

		~MovingAnimation() = default;

	protected:
		unsigned m_Reps = 1;
		int m_Dx = 0, m_Dy = 0;
		unsigned m_Delay = 0;
	};
}