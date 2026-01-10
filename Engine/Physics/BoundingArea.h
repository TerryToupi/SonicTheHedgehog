#pragma once

#include "Core/LatelyDestroyable.h"

namespace physics
{
	using namespace core;

	class BoundingBox;
	class BoundingCircle;

	class BoundingArea : public LatelyDestroyable
	{
	public:
		virtual bool Intersects(const BoundingBox& box) const = 0;
		virtual bool Intersects(const BoundingCircle& circle) const = 0;

		virtual bool In(unsigned x, unsigned y) const = 0;
		virtual bool Intersects(const BoundingArea& area) const = 0;

		virtual BoundingArea* Clone(void) const = 0;

		virtual ~BoundingArea() {}
	};

	class BoundingBox : public BoundingArea
	{
	public:
		virtual bool Intersects(const BoundingBox& box) const override;
		virtual bool Intersects(const BoundingCircle& box) const override;

		virtual bool In(unsigned x, unsigned y) const override;
		virtual bool Intersects(const BoundingArea& area) const override;

		virtual BoundingArea* Clone(void) const override;

		BoundingBox(unsigned _x1, unsigned _y1, unsigned _x2, unsigned _y2);

	public:
		unsigned x1, y1, x2, y2;
	};

	class BoundingCircle : public BoundingArea
	{
	public:
		virtual bool Intersects(const BoundingBox& box) const override;
		virtual bool Intersects(const BoundingCircle& circle) const override;

		bool IntersectsLine(unsigned x1, unsigned y1, unsigned x2, unsigned y2) const;

		virtual bool In(unsigned x, unsigned y) const override;
		virtual bool Intersects(const BoundingArea& area) const override;

		virtual BoundingCircle* Clone(void) const override;

		BoundingCircle(unsigned _x, unsigned _y, unsigned _r);

	public:
		unsigned x, y, r;
	};
}