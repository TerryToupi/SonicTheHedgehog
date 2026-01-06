#pragma once

#include "Utils/Common.h"
#include "Rendering/Bitmap.h"

#include <string>
#include <vector>

namespace anim
{
	using namespace gfx;

	class AnimationFilm
	{
	public:
		struct Data
		{
			std::string			id;
			std::string			path;
			std::vector<Rect>	rects;
		};

		byte				GetTotalFrames(void) const;
		Bitmap				GetBitmap(void) const;
		const std::string&	GetId(void) const;
		const Rect&			GetFrameBox(byte frameNo) const;

		void DisplayFrame(Bitmap d, const Point& at, byte frameNo) const;
		void SetBitmap(Bitmap b);
		void Append(const Rect& r);

		AnimationFilm(const std::string& _id) : m_ID(_id) {}
		AnimationFilm(Bitmap, const std::vector<Rect>&, const std::string&);

	private:
		std::string			m_ID;
		std::vector<Rect>	m_Boxes;
		Bitmap				m_Bitmap = nullptr;
	};
}

