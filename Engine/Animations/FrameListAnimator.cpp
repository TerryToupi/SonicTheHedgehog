#include "Animations/FrameListAnimator.h"
#include "Utils/Assert.h"

namespace anim
{
	FrameListAnimator::FrameListAnimator(void)
		:	Animator()
	{
	}

	void FrameListAnimator::Progress(TimeStamp currtime)
	{
		while (currtime > m_LastTime && (currtime - m_LastTime) >= m_Anim->GetDelay())
		{
			if (m_CurrFrame == m_Anim->GetFrames().back())
			{
				ASSERT(m_Anim->IsForever() || m_CurrRep < m_Anim->GetReps(), "FAILED. FrameList animation was not forever but still keeps playing!");
				m_CurrFrame = m_Anim->GetFrames().at(0);
				m_FrameIndex = 0;
			}
			else
				m_CurrFrame = m_Anim->GetFrames().at(++m_FrameIndex);

			m_LastTime += m_Anim->GetDelay();
			NotifyAction(m_Anim);

			if (m_CurrFrame == m_Anim->GetFrames().back())
			{
				if (!m_Anim->IsForever() && ++m_CurrRep == m_Anim->GetReps())
				{
					m_State = ANIMATOR_FINISHED;
					NotifyStopped();
					return;
				}
			}
		}
	}

	unsigned FrameListAnimator::GetCurrFrame(void) const
	{
		return m_CurrFrame;
	}

	unsigned FrameListAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	auto FrameListAnimator::GetAnim(void) const -> const FrameListAnimation&
	{
		return *m_Anim;
	}

	void FrameListAnimator::Start(FrameListAnimation* a, TimeStamp t, int _currFrame)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrFrame = _currFrame;
		m_CurrRep = 0;
		m_FrameIndex = 0;
		NotifyStarted();
	}
}