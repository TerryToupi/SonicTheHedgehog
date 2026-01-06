#pragma once

#include "Animations/AnimationFilm.h"
#include "Rendering/Bitmap.h"

#include <list>
#include <string>
#include <functional>

namespace anim
{
	using namespace gfx;

	class AnimationFilmHolder
	{
	public:
		using FullParser = std::function<
			bool(std::list<AnimationFilm::Data>& output, const std::string& input)>;
		using EntryParser = std::function<
			int( // -1=error, 0=ended gracefully, else #chars read
				int startPos,
				const std::string& input,
				std::string& idOutput,
				std::string& pathOutput,
				std::vector<Rect>& rectsOutput)>;

	public:
		static auto Get(void) -> AnimationFilmHolder&;

		void Load(const std::string& text, const EntryParser& entryParser);
		void Load(const std::string& text, const FullParser& parser);
		void CleanUp(void);
		auto GetFilm(const std::string& id) -> const AnimationFilm* const;

	private:
		using Films = std::map<std::string, AnimationFilm*>;

		static AnimationFilmHolder s_Holder; // singleton

		Films m_Films;
		BitmapLoader m_Bitmaps;			   // only for loading of film bitmaps

		AnimationFilmHolder(void) {}
		~AnimationFilmHolder() { CleanUp(); }
	};
}
