#pragma once

#include "Utils/Common.h"

#include <functional>

namespace scene
{
	class GravityHandler
	{
	public:
		using OnSolidGroundPred = std::function<bool(const Rect&)>;
		using OnStartFalling = std::function<void(void)>;
		using OnStopFalling = std::function<void(void)>;

	public:
		void SetOnStartFalling(const OnStartFalling& f);
		void SetOnStopFalling(const OnStopFalling& f);
		void SetOnSolidGround(const OnSolidGroundPred& f);
		void SetGravityAddected(bool v);

		void Reset(void);
		void Check(const Rect& r);

	protected:
		bool m_GravityAddicted = false;
		bool m_IsFalling = false;
		OnSolidGroundPred m_OnSolidGround;
		OnStartFalling m_OnStartFalling;
		OnStopFalling m_OnStopFalling;
	};
}