#pragma once

#include "Utils/Common.h"

#include <functional>

namespace scene
{
	class MotionQuantizer
	{
	public:
		using Mover = std::function<void(Rect& r, int* dx, int* dy)>;

	public:
		MotionQuantizer& SetRange(int h, int v);
		MotionQuantizer& SetMover(const Mover& f);
		MotionQuantizer& SetUsed(bool v);
		
		void Move(const Rect& r, int* dx, int* dy);

		MotionQuantizer(void) = default;
		MotionQuantizer(const MotionQuantizer&) = default;

	protected:
		int m_HorizMax = 0;
		int m_VertMax = 0;
		Mover m_Mover;
		bool m_Used = false;
	};
}