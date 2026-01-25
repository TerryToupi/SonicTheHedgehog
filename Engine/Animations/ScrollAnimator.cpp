#include "Animations/ScrollAnimator.h"

namespace anim
{
	ScrollAnimator::ScrollAnimator(void)
		:	Animator()
	{
	}

	void ScrollAnimator::Progress(Time currtime)
	{
		while (currtime > m_LastTime && (currtime - m_LastTime) >= m_Anim->GetScroll().at(m_CurrScroll).delay)
		{
			m_LastTime += m_Anim->GetScroll().at(m_CurrScroll).delay;
			NotifyAction(*m_Anim);
			++m_CurrScroll;
			if (++m_CurrRep == m_Anim->GetScroll().size())
			{
				m_State = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
		}
	}

	unsigned ScrollAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	unsigned ScrollAnimator::GetCurrScroll(void) const
	{
		return m_CurrScroll;
	}

	void ScrollAnimator::Start(ScrollAnimation* a, Time t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrRep = 0;
		NotifyStarted();
	}
}