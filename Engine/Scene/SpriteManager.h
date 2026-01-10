#pragma once

#include<list>
#include<map>
#include<string>

namespace scene
{
	class Sprite;

	class SpriteManager final
	{
	public:
		using SpriteList = std::list<Sprite*>;
		using TypeList = std::map<std::string, SpriteList>;

	public:
		void Add(Sprite* s);
		void Remove(Sprite* s);
		auto GetDisplayList(void) -> const SpriteList&;
		auto GetTypeList(const std::string& typeID) -> const SpriteList&;

		static auto Get(void) -> SpriteManager&;

		SpriteManager(void) = default;
		SpriteManager(const SpriteManager&) = delete;
		SpriteManager(SpriteManager&&) = delete;

	private:
		static SpriteManager s_Manager;

		SpriteList m_Dpylist;
		TypeList   m_Types;
	};
}
