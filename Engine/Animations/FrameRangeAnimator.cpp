#include "Animations/FrameRangeAnimator.h"
#include "Utils/Assert.h"

namespace anim
{
	FrameRangeAnimator::FrameRangeAnimator(void)
		:	Animator()
	{
	}

	void FrameRangeAnimator::Progress(TimeStamp currTime)
	{
		while (currTime > m_LastTime && (currTime - m_LastTime ) >= m_Anim->GetDelay())
		{
			m_PrevFrame = m_CurrFrame;
			if (m_CurrFrame == m_Anim->GetEndFrame())
			{
				ASSERT(m_Anim->IsForever() || m_CurrRep < m_Anim->GetReps(), "FAILD. Framerange animation is out of range!");
				m_CurrFrame = m_Anim->GetStartFrame(); // flip to start
			}
			else
				++m_CurrFrame;

			m_LastTime += m_Anim->GetDelay();
			NotifyAction(m_Anim);

			if (m_CurrFrame == m_Anim->GetEndFrame() && !m_Anim->IsForever() && ++m_CurrRep == m_Anim->GetReps())
			{
				m_State = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
		}
	}

	unsigned FrameRangeAnimator::GetCurrFrame(void) const
	{
		return m_CurrFrame;
	}

	unsigned FrameRangeAnimator::GetPrevFrame(void) const
	{
		return m_PrevFrame;
	}

	unsigned FrameRangeAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	auto FrameRangeAnimator::GetAnim(void) -> const FrameRangeAnimation&
	{
		return *m_Anim;
	}

	bool FrameRangeAnimator::HasJustStarted(void) const
	{
		return m_CurrFrame == m_Anim->GetStartFrame() && !m_CurrRep;
	}

	void FrameRangeAnimator::Start(FrameRangeAnimation* a, TimeStamp t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrFrame = m_PrevFrame = m_Anim->GetStartFrame();
		m_CurrRep = 0;
		NotifyStarted();
	}
}
