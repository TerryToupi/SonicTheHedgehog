#include "Scene/MotionQuantizer.h"

namespace scene
{
	MotionQuantizer& MotionQuantizer::SetRange(int h, int v)
	{
		m_HorizMax = h;
		m_VertMax = v;
		m_Used = true;
		return *this;
	}

	MotionQuantizer& MotionQuantizer::SetMover(const Mover& f)
	{
		m_Mover = f;
		return *this;
	}

	MotionQuantizer& MotionQuantizer::SetUsed(bool v)
	{
		m_Used = v;
		return *this;
	}

	void MotionQuantizer::Move(const Rect& r, int* dx, int* dy)
	{
		if (!m_Used)
			m_Mover(r, dx, dy);
		else
			do
			{
				auto sign_x = number_sign(*dx);
				auto sign_y = number_sign(*dy);
				auto dxFinal = sign_x * std::min(m_HorizMax, sign_x * *dx);
				auto dyFinal = sign_y * std::min(m_VertMax, sign_y * *dy);

				m_Mover(r, &dxFinal, &dyFinal);

				if (!dxFinal)
					*dx = 0;
				else
					*dx -= dxFinal;

				if (!dyFinal)
					*dy = 0;
				else
					*dy -= dyFinal;
			} while (*dx || *dy);
	}
}


