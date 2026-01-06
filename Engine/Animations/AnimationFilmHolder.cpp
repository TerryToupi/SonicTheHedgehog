#include "Animations/AnimationFilmHolder.h"
#include "Utils/Assert.h"

namespace anim
{
	const AnimationFilmHolder& AnimationFilmHolder::Get(void)
	{
		return s_Holder;
	}

	void AnimationFilmHolder::Load(const std::string& text, const EntryParser& entryParser)
	{
		int pos = 0;
		while (true)
		{
			std::string id, path;
			std::vector<Rect> rects;
			auto i = entryParser(pos, text, id, path, rects);
			ASSERT(i >= 0, "Failed. No insert in the animation film Holder!");
			if (!i)
				return;
			pos += i;
			ASSERT(!GetFilm(id), "Failed. Film already inserted in animation film holder!");
			m_Films[id] = new AnimationFilm(m_Bitmaps.Load(path), rects, id);
		}
	}

	void AnimationFilmHolder::Load(const std::string& text, const FullParser& parser)
	{
		std::list<AnimationFilm::Data> output;
		auto result = parser(output, text);
		ASSERT(result, "Failed. Parser provided in Animation Film holder return invalid Result");

		for (auto& entry : output)
		{
			ASSERT(!GetFilm(entry.id), "Failed. Film already inserted in animation film holder!");
			m_Films[entry.id] = new AnimationFilm(m_Bitmaps.Load(entry.path), entry.rects, entry.id);
		}
	}

	void AnimationFilmHolder::CleanUp(void)
	{
		for (auto& i : m_Films)
			delete (i.second);
		m_Films.clear();
	}

	auto AnimationFilmHolder::GetFilm(const std::string& id) -> const AnimationFilm* const
	{
		auto i = m_Films.find(id);
		return i != m_Films.end() ? i->second : nullptr;
	}
}