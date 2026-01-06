#include "Animations/TickAnimation.h"
#include "Utils/Assert.h"

namespace anim
{
	unsigned TickAnimation::GetDelay(void) const
	{
		return m_Delay;
	}

	TickAnimation::Me& TickAnimation::SetDelay(unsigned v)
	{
		m_Delay = v;
		return *this;
	}

	unsigned TickAnimation::GetReps(void) const
	{
		return m_Reps;
	}

	TickAnimation::Me& TickAnimation::SetReps(unsigned n)
	{
		m_Reps = n;
		return *this;
	}

	bool TickAnimation::IsForever(void) const
	{
		return !m_Reps;
	}

	TickAnimation::Me& TickAnimation::SetForever(void)
	{
		m_Reps = 0;
		return *this;
	}

	bool TickAnimation::IsDiscrete(void) const
	{
		return m_IsDiscrete;
	}

	anim::Animation* TickAnimation::Clone(void) const
	{
		return new TickAnimation(m_ID, m_Delay, m_Reps, m_IsDiscrete);
	}

	TickAnimation::TickAnimation(const std::string& id, unsigned d, unsigned r, bool discrete)
		:	Animation(id), m_Delay(d), m_Reps(r), m_IsDiscrete(discrete)
	{
		ASSERT(Inv(), "FAILED. Tick animation was not Inv!");
	}

	bool TickAnimation::Inv(void) const
	{
		return m_IsDiscrete || m_Reps == 1;
	}
}