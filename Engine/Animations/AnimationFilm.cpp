#include "Animations/AnimationFilm.h"
#include "Utils/Assert.h"

namespace anim
{
	byte AnimationFilm::GetTotalFrames(void) const
	{
		return (byte)m_Frames.size();
	}

	Bitmap AnimationFilm::GetBitmap(void) const
	{
		return m_Bitmap;
	}

	const std::string& AnimationFilm::GetId(void) const
	{
		return m_ID;
	}

	const Rect& AnimationFilm::GetFrameBox(byte frameNo) const
	{
		ASSERT(m_Frames.size() > frameNo, "Failed. Frame number is out of bounds!");
		return m_Frames[frameNo].rect;
	}

	const Point& AnimationFilm::GetFrameOffset(byte frameNo) const
	{
		ASSERT(m_Frames.size() > frameNo, "Failed. Frame number is out of bounds!");
		return m_Frames[frameNo].offset;
	}

	void AnimationFilm::DisplayFrame(Bitmap d, const Point& at, byte frameNo) const
	{
		const Point& offset = GetFrameOffset(frameNo);
		Point adjustedPos = { at.x + offset.x, at.y + offset.y };
		BitmapBlit(m_Bitmap, GetFrameBox(frameNo), d, adjustedPos);
	}

	void AnimationFilm::SetBitmap(Bitmap b)
	{
		ASSERT(!m_Bitmap, "Failed, Bitmap is already set!");
		m_Bitmap = b;
	}

	void AnimationFilm::Append(const FrameData& frame)
	{
		m_Frames.push_back(frame);
	}

	AnimationFilm::AnimationFilm(Bitmap b, const std::vector<FrameData>& frames, const std::string& id)
		:	m_Bitmap(b), m_Frames(frames), m_ID(id)
	{
	}
}