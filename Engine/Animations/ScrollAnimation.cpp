#include "Animations/ScrollAnimation.h"

namespace anim
{
	const ScrollAnimation::Scroll& ScrollAnimation::GetScroll(void) const
	{
		return m_Scroll;
	}

	ScrollAnimation::Me& ScrollAnimation::SetScroll(const Scroll& p)
	{
		m_Scroll = p;
		return *this;
	}

	Animation* ScrollAnimation::Clone(void) const
	{
		return new ScrollAnimation(m_ID, m_Scroll);
	}

	ScrollAnimation::ScrollAnimation(const std::string& id, const Scroll& scroll)
		:	Animation(id), m_Scroll(scroll)
	{
	}
}