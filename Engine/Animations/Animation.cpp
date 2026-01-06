#include "Animations/Animation.h"

namespace anim
{
	const std::string& Animation::GetID()
	{
		return m_ID;
	}

	void Animation::SetID(const std::string& id)
	{
		m_ID = id;
	}

	Animation::Animation(const std::string& id)
		: m_ID(id)
	{
	}
}