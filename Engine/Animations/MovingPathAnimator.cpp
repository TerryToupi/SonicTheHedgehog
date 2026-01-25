#include "Animations/MovingPathAnimator.h"

namespace anim
{
	MovingPathAnimator::MovingPathAnimator(void)
		:	Animator()
	{
	}

	void MovingPathAnimator::Progress(TimeStamp currTime)
	{
		while (currTime > m_LastTime && (currTime - m_LastTime) >= m_Anim->GetPath().at(m_CurrPath).delay)
		{
			m_LastTime += m_Anim->GetPath().at(m_CurrPath).delay;
			NotifyAction(m_Anim);
			++m_CurrPath;
			if (++m_CurrRep == m_Anim->GetPath().size())
			{
				m_State = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
		}
	}

	unsigned MovingPathAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	unsigned MovingPathAnimator::GetCurrPath(void) const
	{
		return m_CurrPath;
	}

	auto MovingPathAnimator::GetAnim(void) const -> const MovingPathAnimation&
	{
		return *m_Anim;
	}

	void MovingPathAnimator::Start(MovingPathAnimation* a, TimeStamp t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrRep = 0;
		m_CurrPath = 0;
		NotifyStarted();
	}
}