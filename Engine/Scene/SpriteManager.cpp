#include "Scene/SpriteManager.h"
#include "Scene/Sprite.h"

namespace scene
{
	SpriteManager SpriteManager::s_Manager;

	void SpriteManager::Add(Sprite* s1)
	{
		auto i = std::find_if(
			m_Dpylist.begin(), m_Dpylist.end(),
			[s1](Sprite* s2)
			{ return s2->GetZorder() > s1->GetZorder(); });
		m_Dpylist.insert(i, s1);
		if (!s1->GetTypeID().empty())
			m_Types[s1->GetTypeID()].push_back(s1);
	}

	void SpriteManager::Remove(Sprite* s)
	{
		m_Dpylist.remove(s);
		if (!s->GetTypeID().empty())
			m_Types[s->GetTypeID()].remove(s);

		s->Destroy();
	}

	auto SpriteManager::GetDisplayList(void) -> const SpriteList&
	{
		return m_Dpylist;
	}

	auto SpriteManager::GetTypeList(const std::string& typeID) -> const SpriteList&
	{
		return m_Types[typeID];
	}

	auto SpriteManager::Get(void) -> SpriteManager&
	{
		return s_Manager;
	}
}