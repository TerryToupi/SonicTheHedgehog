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
		struct ColorKey
		{
			RGBValue r = 0, g = 0, b = 0;
			bool enabled = false;
		};

		struct FrameData
		{
			Rect	rect;			// Source rectangle in spritesheet
			Point	offset = {0, 0};	// Display offset for centering
		};

		struct Data
		{
			std::string				id;
			std::string				path;
			std::vector<FrameData>	frames;
			ColorKey				colorKey;
		};

		byte				GetTotalFrames(void) const;
		Bitmap				GetBitmap(void) const;
		const std::string&	GetId(void) const;
		const Rect&			GetFrameBox(byte frameNo) const;
		const Point&		GetFrameOffset(byte frameNo) const;

		void DisplayFrame(Bitmap d, const Point& at, byte frameNo) const;
		void SetBitmap(Bitmap b);
		void Append(const FrameData& frame);

		AnimationFilm(const std::string& _id) : m_ID(_id) {}
		AnimationFilm(Bitmap, const std::vector<FrameData>&, const std::string&);

	private:
		std::string				m_ID;
		std::vector<FrameData>	m_Frames;
		Bitmap					m_Bitmap = nullptr;
	};
}

