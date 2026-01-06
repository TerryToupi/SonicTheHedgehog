#include "Animations/FlashAnimation.h"

namespace anim
{
	FlashAnimation::Me& FlashAnimation::SetReps(unsigned n)
	{
		m_Repetitions = n;
		return *this;
	}

	unsigned FlashAnimation::GetReps(void) const
	{
		return m_Repetitions;
	}

	FlashAnimation::Me& FlashAnimation::SetHideDelay(unsigned n)
	{
		m_HideDelay = n;
		return *this;
	}

	unsigned FlashAnimation::GetHideDelay(void) const
	{
		return m_HideDelay;
	}

	FlashAnimation::Me& FlashAnimation::SetShowDelay(unsigned n)
	{
		m_ShowDelay = n;
		return *this;
	}

	unsigned FlashAnimation::GetShowDelay(void) const
	{
		return m_ShowDelay;
	}

	Animation* FlashAnimation::Clone(void) const
	{
		return new FlashAnimation(m_ID, m_Repetitions, m_HideDelay, m_ShowDelay);
	}

	FlashAnimation::FlashAnimation(const std::string& id, unsigned n, unsigned show, unsigned hide)
		:	Animation(id), m_Repetitions(n), m_HideDelay(hide), m_ShowDelay(show)
	{
	}
}