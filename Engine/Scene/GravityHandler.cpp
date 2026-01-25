#include "Scene/GravityHandler.h"

namespace scene
{
	void GravityHandler::SetOnStartFalling(const OnStartFalling& f)
	{
		m_OnStartFalling = f;
	}

	void GravityHandler::SetOnStopFalling(const OnStopFalling& f)
	{
		m_OnStopFalling = f;
	}

	void GravityHandler::SetOnSolidGround(const OnSolidGroundPred& f)
	{
		m_OnSolidGround = f;
	}

	void GravityHandler::SetGravityAddected(bool v)
	{
		m_GravityAddicted = v;
	}

	void GravityHandler::Reset(void)
	{
		m_IsFalling = false;
	}

	void GravityHandler::Check(Rect& r)
	{
		if (m_GravityAddicted)
		{
			if (m_OnSolidGround(r))
			{
				if (m_IsFalling)
				{
					m_IsFalling = false;
					m_OnStopFalling();
				}
			}
			else if (!m_IsFalling)
			{
				m_IsFalling = true;
				m_OnStartFalling();
			}
		}
	}
}
