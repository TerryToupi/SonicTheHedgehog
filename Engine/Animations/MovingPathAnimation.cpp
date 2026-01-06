#include "Animations/MovingPathAnimation.h"

namespace anim
{
	const anim::MovingPathAnimation::Path& MovingPathAnimation::GetPath(void) const
	{
		return m_Path;
	}

	MovingPathAnimation::Me& MovingPathAnimation::SetPath(const Path& p)
	{
		m_Path = p;
		return *this;
	}

	Animation* MovingPathAnimation::Clone(void) const
	{
		return new MovingPathAnimation(m_ID, m_Path);
	}

	MovingPathAnimation::MovingPathAnimation(const std::string& id, const Path& path)
		:	m_Path(path), Animation(m_ID)
	{
	}
}