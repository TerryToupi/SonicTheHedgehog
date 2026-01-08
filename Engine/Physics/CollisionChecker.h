#pragma once

#include <list>
#include <functional>

namespace physics
{
	class Sprite;

	class CollisionChecker final
	{
	public:
		using Action = std::function<void(Sprite* s1, Sprite* s2)>;

	protected:
		using Entry = std::tuple<Sprite*, Sprite*, Action>;

	public:
		void Register(Sprite* s1, Sprite* s2, const Action& f);
		void Cancel(Sprite* s1, Sprite* s2);
		void Check(void) const;

		static auto Get(void) -> CollisionChecker&;

		CollisionChecker(void) = default;
		CollisionChecker(const CollisionChecker&) = delete;
		CollisionChecker(CollisionChecker&&) = delete;

	protected:
		auto Find(Sprite* s1, Sprite* s2) -> std::list<Entry>::iterator;
		bool In(Sprite* s1, Sprite* s2);

	protected:
		static CollisionChecker s_Checker;

		std::list<Entry> m_Entries;
	};
}