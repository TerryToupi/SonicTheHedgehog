#include "Animations/MovingAnimation.h"

namespace anim
{
	int MovingAnimation::GetDx(void) const
	{
		return m_Dx;
	}

	anim::MovingAnimation::Me& MovingAnimation::SetDx(int v)
	{
		m_Dx = v;
		return *this;
	}

	int MovingAnimation::GetDy(void) const
	{
		return m_Dy;
	}

	anim::MovingAnimation::Me& MovingAnimation::SetDy(int v)
	{
		m_Dy = v;
		return *this;
	}

	unsigned MovingAnimation::GetDelay(void) const
	{
		return m_Delay;
	}

	anim::MovingAnimation::Me& MovingAnimation::SetDelay(unsigned v)
	{
		m_Delay = v;
		return *this;
	}

	unsigned MovingAnimation::GetReps(void) const
	{
		return m_Reps;
	}

	anim::MovingAnimation::Me& MovingAnimation::SetReps(unsigned n)
	{
		m_Reps = n;
		return *this;
	}

	bool MovingAnimation::IsForever(void) const
	{
		return !m_Reps;
	}

	anim::MovingAnimation::Me& MovingAnimation::SetForever(void)
	{
		m_Reps = 0;
		return *this;
	}

	bool MovingAnimation::IsAnalogue(void) const
	{
		return !m_Reps && !m_Delay;
	}

	anim::Animation* MovingAnimation::Clone(void) const
	{
		return new MovingAnimation(m_ID, m_Reps, m_Dx, m_Dy, m_Delay);
	}

	MovingAnimation::MovingAnimation(const std::string& id, unsigned reps, int dx, int dy, unsigned delay)
		:	Animation(id), m_Reps(reps), m_Dx(dx), m_Dy(dy), m_Delay(delay)
	{
	}

	MovingAnimation::MovingAnimation(int dx, int dy)
		:	Animation("${MovingAnimation}"), m_Reps(0), m_Dx(dx), m_Dy(dy), m_Delay(0)
	{
	}
}