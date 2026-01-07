#include "Animations/MovingAnimator.h"

namespace anim
{
	void MovingAnimator::Progress(TimeStamp currTime)
	{
		while (currTime > m_LastTime && (currTime - m_LastTime) >= m_Anim->GetDelay())
		{
			m_LastTime += m_Anim->GetDelay();
			NotifyAction(*m_Anim);
			if (!m_Anim->IsForever() && ++m_CurrRep == m_Anim->GetReps())
			{
				m_State = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
		}
	}

	void MovingAnimator::ProgressContinuous(TimeStamp currTime)
	{
		auto vx = float(m_Anim->GetDx()) / float(m_Anim->GetDelay());
		auto vy = float(m_Anim->GetDy()) / float(m_Anim->GetDelay());
		auto dt = float(currTime - m_LastTime);
		m_LastTime = currTime;
		NotifyAction(
			MovingAnimation(vx * dt, vy * dt)
		);
	}

	auto MovingAnimator::GetAnim(void) const -> const MovingAnimation&
	{
		return *m_Anim;
	}

	void MovingAnimator::Start(MovingAnimation* a, TimeStamp t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrRep = 0;
		NotifyStarted();
	}
}