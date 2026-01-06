#include "Animations/FrameRangeAnimation.h"

namespace anim
{
	unsigned FrameRangeAnimation::GetStartFrame(void) const
	{
		return m_Start;
	}

	FrameRangeAnimation::Me& FrameRangeAnimation::SetStartFrame(unsigned v)
	{
		m_Start = v;
		return *this;
	}

	unsigned FrameRangeAnimation::GetEndFrame(void) const
	{
		return m_End;
	}

	FrameRangeAnimation::Me& FrameRangeAnimation::SetEndFrame(unsigned v)
	{
		m_End = v;
		return *this;
	}

	anim::Animation* FrameRangeAnimation::Clone(void) const
	{
		return new FrameRangeAnimation(
			m_ID, m_Start, m_End, GetReps(), GetDx(), GetDy(), GetDelay()
		);
	}

	FrameRangeAnimation::FrameRangeAnimation(const std::string& id, unsigned s, unsigned e, unsigned r, int dx, int dy, int d)
		:	m_Start(s), m_End(e), MovingAnimation(id, r, dx, dy, d)
	{
	}
}
