#include "Animations/TickAnimator.h"

namespace anim
{
	TickAnimator::TickAnimator(void)
		:	Animator()
	{
	}

	void TickAnimator::Progress(TimeStamp currTime)
	{
		if (!m_Anim->IsDiscrete())
		{
			m_ElapsedTime = currTime - m_LastTime;
			m_LastTime = currTime;
			NotifyAction(m_Anim);
		}
		else
			while (currTime > m_LastTime && (currTime - m_LastTime) >= m_Anim->GetDelay())
			{

				m_LastTime += m_Anim->GetDelay();
				NotifyAction(m_Anim);

				if (!m_Anim->IsForever() && ++m_CurrRep == m_Anim->GetReps())
				{
					m_State = ANIMATOR_FINISHED;
					NotifyStopped();
					return;
				}
			}
	}

	unsigned TickAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	unsigned TickAnimator::GetElapsedTime(void) const
	{
		return m_ElapsedTime;
	}

	float TickAnimator::GetElapsedTimeNormalised(void) const
	{
		return float(m_ElapsedTime) / float(m_Anim->GetDelay());
	}

	auto TickAnimator::GetAnim(void) const -> const TickAnimation&
	{
		return *m_Anim;
	}

	void TickAnimator::Start(TickAnimation* a, TimeStamp t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrRep = 0;
		m_ElapsedTime = 0;
		NotifyStarted();
	}
}