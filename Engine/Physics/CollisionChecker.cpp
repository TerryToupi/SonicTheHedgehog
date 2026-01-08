#include "Physics/CollisionChecker.h"
#include "Utils/Assert.h"

#include <algorithm>

namespace physics
{
	CollisionChecker CollisionChecker::s_Checker;

	void CollisionChecker::Register(Sprite* s1, Sprite* s2, const Action& f)
	{
		ASSERT(!In(s1, s2), "FAILED, sprites have already been registered for collision!");
	}

	void CollisionChecker::Cancel(Sprite* s1, Sprite* s2) 
	{
		m_Entries.erase(Find(s1, s2));
	}

	void CollisionChecker::Check(void) const
	{
		//TODO: implement once we have sprites
	}

	auto CollisionChecker::Get(void) -> CollisionChecker&
	{
		return s_Checker;
	}

	auto CollisionChecker::Find(Sprite* s1, Sprite* s2) -> std::list<Entry>::iterator
	{
		return std::find_if(
			m_Entries.begin(),
			m_Entries.end(),
			[s1, s2](const Entry& e) {
				return std::get<0>(e) == s1 && std::get<1>(e) == s2 ||
					std::get<0>(e) == s2 && std::get<1>(e) == s1;
			});
	}

	bool CollisionChecker::In(Sprite* s1, Sprite* s2)
	{
		return Find(s1, s2) != m_Entries.end();
	}
}

