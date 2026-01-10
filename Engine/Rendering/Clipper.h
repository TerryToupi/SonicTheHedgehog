#pragma once

#include "Utils/Common.h"

#include <functional>

namespace gfx
{
	class Clipper
	{
	public:
		using View = std::function<const Rect& (void)>;

	public:
		Clipper& SetView(const View& f);
		bool Clip(const Rect& r, const Rect& dpyArea, Point* dpyPos, Rect* clippedBox) const;

		Clipper(void) = default;
		Clipper(const Clipper&) = default;

	private:
		View m_View;
	};
}