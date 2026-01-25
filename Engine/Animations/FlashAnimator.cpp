#include "Animations/FlashAnimator.h"

namespace anim
{
	FlashHideAnimator::FlashHideAnimator(void)
		:	Animator()
	{
	}

	FlashShowAnimator::FlashShowAnimator(void)
		:	Animator()
	{
	}

	void FlashShowAnimator::Progress(TimeStamp currtime)
	{
		while (currtime > m_LastTime && (currtime - m_LastTime) >= m_Anim->GetShowDelay())
		{
			m_LastTime += m_Anim->GetShowDelay();
			NotifyAction(m_Anim);
			if (++m_CurrRep == m_Anim->GetReps())
			{
				m_State = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
		}
	}

	unsigned FlashShowAnimator::GetShowDelay(void) const
	{
		return m_ShowDelay;
	}

	unsigned FlashShowAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	auto FlashShowAnimator::GetAnim(void) const -> const FlashAnimation&
	{
		return *m_Anim;
	}

	void FlashShowAnimator::Start(FlashAnimation* a, TimeStamp t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrRep = 0;
		NotifyStarted();
	}

	void FlashHideAnimator::Progress(TimeStamp currtime)
	{
		while (currtime > m_LastTime && (currtime - m_LastTime) >= m_Anim->GetHideDelay())
		{
			m_LastTime += m_Anim->GetHideDelay();
			NotifyAction(m_Anim);
			if (++m_CurrRep == m_Anim->GetReps())
			{
				m_State = ANIMATOR_FINISHED;
				NotifyStopped();
				return;
			}
		}
	}

	unsigned FlashHideAnimator::GetHideDelay(void) const
	{
		return m_HideDelay;
	}

	unsigned FlashHideAnimator::GetCurrRep(void) const
	{
		return m_CurrRep;
	}

	auto FlashHideAnimator::GetAnim(void) const -> const FlashAnimation&
	{
		return *m_Anim;
	}

	void FlashHideAnimator::Start(FlashAnimation* a, TimeStamp t)
	{
		m_Anim = a;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_CurrRep = 0;
		NotifyStarted();
	}
}