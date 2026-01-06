#include "Animations/FrameListAnimation.h"

namespace anim
{

	const FrameListAnimation::Frames& FrameListAnimation::GetFrames(void) const
	{
		return m_Frames;
	}

	FrameListAnimation::Me& FrameListAnimation::SetFrames(const Frames& f)
	{
		m_Frames = f;
		return *this;
	}

	Animation* FrameListAnimation::Clone(void) const
	{
		return new FrameListAnimation(
			m_ID, m_Frames, GetReps(), GetDx(), GetDy(), GetDelay()
		);
	}

	FrameListAnimation::FrameListAnimation(const std::string& id, const Frames& frames, unsigned r, int dx, int dy, unsigned d)
		:	m_Frames(frames), MovingAnimation(m_ID, r, dx, dy, d)
	{
	}
}