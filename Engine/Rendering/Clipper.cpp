#include "Rendering/Clipper.h"

namespace gfx
{
	template <class T>
	static inline bool clip_function(
		T x, T y, T w, T h,
		T wx, T wy, T ww, T wh,
		T* cx, T* cy, T* cw, T* ch)
	{
		*cw = T(std::min(wx + ww, x + w)) - (*cx = T(std::max(wx, x)));
		*ch = T(std::min(wy + wh, y + h)) - (*cy = T(std::max(wy, y)));
		return *cw > 0 && *ch > 0;
	}

	static inline bool clip_rect(const Rect& r, const Rect& area, Rect* result)
	{
		return clip_function(
			r.x,
			r.y,
			r.w,
			r.h,
			area.x,
			area.y,
			area.w,
			area.h,
			&result->x,
			&result->y,
			&result->w,
			&result->h
		);
	}

	Clipper& Clipper::SetView(const View& f)
	{
		m_View = f;
		return *this;
	}

	bool Clipper::Clip(const Rect& r, const Rect& dpyArea, Point* dpyPos, Rect* clippedBox) const
	{
		Rect visibleArea;
		if (!clip_rect(r, m_View(), &visibleArea))
		{
			clippedBox->w = clippedBox->h = 0;
			return false;
		}
		else
		{

			clippedBox->x = r.x - visibleArea.x;
			clippedBox->y = r.y - visibleArea.y;

			clippedBox->w = visibleArea.w;
			clippedBox->h = visibleArea.h;

			dpyPos->x = dpyArea.x + (visibleArea.x - m_View().x);
			dpyPos->y = dpyArea.y + (visibleArea.y - m_View().y);

			return true;
		}
	}
}

