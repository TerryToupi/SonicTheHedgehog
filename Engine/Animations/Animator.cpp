#include "Animations/Animator.h"
#include "Animations/AnimatorManager.h"

namespace anim
{
	void Animator::Stop(void)
	{
		Finish(true);
	}

	bool Animator::HasFinished(void) const
	{
		return m_State != ANIMATOR_RUNNING;
	}

	void Animator::TimeShift(TimeStamp offset)
	{
		m_LastTime += offset;
	}

	void Animator::SetOnFinish(const OnFinish& f)
	{
		m_OnFinish = f;
	}

	void Animator::SetOnAction(const OnAction& f)
	{
		m_OnAction = f;
	}

	void Animator::SetOnStart(const OnStart& f)
	{
		m_OnStart = f;
	}

	Animator::Animator(void)
	{
		AnimatorManager::Get().Register(this);
	}

	Animator::~Animator(void)
	{
		AnimatorManager::Get().Cancel(this);
	}

	void Animator::NotifyStopped(void)
	{
		AnimatorManager::Get().MarkAsSuspended(this);
		if (m_OnFinish)
			(m_OnFinish)(this);
	}

	void Animator::NotifyStarted(void)
	{
		AnimatorManager::Get().MarkAsRunning(this);
		if (m_OnStart)
			(m_OnStart)(this);
	}

	void Animator::NotifyAction(Animation* anim)
	{
		if (m_OnAction)
			(m_OnAction)(this, anim);
	}

	void Animator::Finish(bool isForced /*= false*/)
	{
		if (!HasFinished())
		{
			m_State = isForced ? ANIMATOR_STOPPED : ANIMATOR_FINISHED;
			NotifyStopped();
		}
	}
}