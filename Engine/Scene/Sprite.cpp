#pragma once

#include "Scene/Sprite.h"
#include "Utils/Assert.h"

namespace scene
{
	void Sprite::SetMover(const Mover& f)
	{
		m_Quantizer.SetMover(m_Mover = f);
	}

	const Rect Sprite::GetBox(void) const
	{
		return { m_X, m_Y, m_FrameBox.w, m_FrameBox.h };
	}

	GravityHandler& Sprite::GetGravityHandler(void)
	{
		return m_Gravity;
	}

	void Sprite::SetHasDirectMotion(bool v)
	{
		m_DirectMotion = v;
	}

	bool Sprite::GetHasDirectMotion(void)
	{
		return m_DirectMotion;
	}

	void Sprite::Move(int dx, int dy)
	{
		if (m_DirectMotion)
			MoveDirect(dx, dy);
		else
		{
			auto r = GetBox();
			m_Quantizer.Move(r, &dx, &dy);
			m_Gravity.Check(r);
		}
	}

	void Sprite::MoveDirect(int dx, int dy)
	{
		m_X += dx;
		m_Y += dy;
	}

	void Sprite::SetPos(int _x, int _y)
	{
		m_X = _x;
		m_Y = _y;
	}

	void Sprite::SetZorder(unsigned z)
	{
		m_Zorder = z;
	}

	unsigned Sprite::GetZorder(void)
	{
		return m_Zorder;
	}

	void Sprite::SetFrame(byte i)
	{
		ASSERT(m_CurrFilm, "FAILED. Can't set frame without a film");

		if (i != m_FrameNo)
		{
			ASSERT(i < m_CurrFilm->GetTotalFrames(), "FAILED. the requested frame is out of bounds of the film!");
			m_FrameBox = m_CurrFilm->GetFrameBox(m_FrameNo = i);
		}
	}

	void Sprite::SetBoundingArea(BoundingArea* area)
	{
		ASSERT(!m_BoundingArea, "FAILED. Bounding area is already been set!");
		m_BoundingArea = area;
	}

	auto Sprite::GetBoundingArea(void) const -> const BoundingArea*
	{
		return m_BoundingArea;
	}

	byte Sprite::GetFrame(void)
	{
		return m_FrameNo;
	}

	void Sprite::SetTypeID(const std::string& _id)
	{
		m_TypeID = _id;
	}

	auto Sprite::GetTypeID(void) -> const std::string&
	{
		return m_TypeID;
	}

	void Sprite::SetVisibility(bool v)
	{
		m_IsVisible = v;
	}

	bool Sprite::IsVisible(void) const
	{
		return m_IsVisible;
	}

	bool Sprite::CollisionCheck(const Sprite* s) const
	{
		ASSERT(m_BoundingArea, "FAILED. Sprite does not have a bounding area!");
		ASSERT(s->m_BoundingArea, "FAILED. Given Sprite does not have a bounding area!");
		
		return m_BoundingArea->Intersects(*s->GetBoundingArea());
	}

	void Sprite::Display(Bitmap dest, const Rect& dpyArea, const Clipper& clipper) const
	{
		ASSERT(m_CurrFilm, "FAILED. Can't display a sprite without a film!");

		Rect clippedBox;
		Point dpyPos;

		if (clipper.Clip(GetBox(), dpyArea, &dpyPos, &clippedBox))
		{
			Rect clippedFrame = {
				m_FrameBox.x + clippedBox.x,
				m_FrameBox.y + clippedBox.y,
				clippedBox.w,
				clippedBox.h
			};

			// Apply frame offset for centering (e.g., thin frames in animation)
			const Point& offset = m_CurrFilm->GetFrameOffset(m_FrameNo);
			Point adjustedPos = { dpyPos.x + offset.x, dpyPos.y + offset.y };

			BitmapBlit(
				m_CurrFilm->GetBitmap(),
				clippedFrame,
				dest,
				adjustedPos
			);
		}
	}

	void Sprite::SetFilm(AnimationFilm* film)
	{
		m_CurrFilm = film;
	}

	auto Sprite::GetFilm(void) const -> const AnimationFilm&
	{
		ASSERT(m_CurrFilm, "FAILED. Film in not set!");
		return *m_CurrFilm;
	}

	Sprite::Sprite(int _x, int _y, const std::string& _typeID /*= ""*/)
		:	m_X(_x), m_Y(_y), m_CurrFilm(nullptr), m_FrameNo(0), m_TypeID(_typeID), m_FrameBox({})
	{
		m_Quantizer.SetMover(
			[this](const Rect& r, int* dx, int* dy)
			{
				ASSERT(r == GetBox(), "FAILED. Provided rect did not match the dimentions of the sprite!");
				MoveDirect(*dx, *dy);
			}
		);
	}
}

