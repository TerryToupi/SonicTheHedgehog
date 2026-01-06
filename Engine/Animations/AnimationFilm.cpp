#include "Animations/AnimationFilm.h" 

#include "Utils/Assert.h"

namespace anim
{

	byte AnimationFilm::GetTotalFrames(void) const
	{
		return (byte)m_Boxes.size();
	}

	gfx::Bitmap AnimationFilm::GetBitmap(void) const
	{
		return m_Bitmap;
	}

	const std::string& AnimationFilm::GetId(void) const
	{
		return m_ID;
	}

	const Rect& AnimationFilm::GetFrameBox(byte frameNo) const
	{
		ASSERT(m_Boxes.size() > frameNo, "Failed. Frame number is out of bounds!");
		return m_Boxes[frameNo];
	}

	void AnimationFilm::DisplayFrame(Bitmap d, const Point& at, byte frameNo) const
	{
		BitmapBlit(m_Bitmap, GetFrameBox(frameNo), d, at);
	}

	void AnimationFilm::SetBitmap(Bitmap b)
	{
		ASSERT(!m_Bitmap, "Failed, Bitmap is already set!");
		m_Bitmap = b;
	}

	void AnimationFilm::Append(const Rect& r)
	{
		m_Boxes.push_back(r);
	}

	AnimationFilm::AnimationFilm(Bitmap b, const std::vector<Rect>& boxes, const std::string& id)
		:	m_Bitmap(b), m_Boxes(boxes), m_ID(id)
	{
	}
}