#pragma once

#include "Utils/Common.h"
#include "Core/LatelyDestroyable.h"
#include "Animations/Animation.h"

#include <functional>

namespace anim
{
	using namespace core;

	class Animator : public LatelyDestroyable
	{
	public:
		enum animatorstate_t
		{
			ANIMATOR_FINISHED = 0,
			ANIMATOR_RUNNING = 1,
			ANIMATOR_STOPPED = 2
		};

	public:
		using OnFinish = std::function<void(Animator*)>;
		using OnStart = std::function<void(Animator*)>;
		using OnAction = std::function<void(Animator*, const Animation&)>;
		
	public:
		void Stop(void);
		bool HasFinished(void) const;

		virtual void TimeShift(TimeStamp offset);
		virtual void Progress(TimeStamp currTime) = 0;

		void SetOnFinish(const OnFinish& f);
		void SetOnAction(const OnAction& f);
		void SetOnStart(const OnStart& f);

		Animator(void);
		virtual ~Animator(void);
		Animator(const Animator&) = delete;
		Animator(Animator&&) = delete;

	protected:
		void NotifyStopped(void);
		void NotifyStarted(void);
		void NotifyAction(const Animation&);
		void Finish(bool isForced = false);

	protected:
		TimeStamp m_LastTime = 0;
		animatorstate_t m_State = ANIMATOR_FINISHED;
		OnFinish m_OnFinish;
		OnStart m_OnStart;
		OnAction m_OnAction;
	};
}