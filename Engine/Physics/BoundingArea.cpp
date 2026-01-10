#include "Physics/BoundingArea.h"

#include <algorithm>
#include <cmath>

namespace physics
{
	BoundingBox::BoundingBox(unsigned _x1, unsigned _y1, unsigned _x2, unsigned _y2)
	{
		x1 = std::min(_x1, _x2);
		y1 = std::min(_y1, _y2);
		x2 = std::max(_x1, _x2);
		y2 = std::max(_y1, _y2);
	}

	bool BoundingBox::In(unsigned x, unsigned y) const
	{
		return x >= x1 && x <= x2 &&
			y >= y1 && y <= y2;
	}

	bool BoundingBox::Intersects(const BoundingBox& box) const
	{
		return !(x2 < box.x1 || x1 > box.x2 ||
			y2 < box.y1 || y1 > box.y2);
	}

	bool BoundingBox::Intersects(const BoundingCircle& circle) const
	{
		unsigned closestX = std::clamp(circle.x, x1, x2);
		unsigned closestY = std::clamp(circle.y, y1, y2);

		int dx = int(circle.x) - int(closestX);
		int dy = int(circle.y) - int(closestY);

		return (dx * dx + dy * dy) <= int(circle.r * circle.r);
	}

	bool BoundingBox::Intersects(const BoundingArea& area) const
	{
		return area.Intersects(*this);
	}

	BoundingArea* BoundingBox::Clone() const
	{
		return new BoundingBox(*this);
	}

	BoundingCircle::BoundingCircle(unsigned _x, unsigned _y, unsigned _r)
		: x(_x), y(_y), r(_r)
	{
	}

	bool BoundingCircle::In(unsigned x, unsigned y) const
	{
		int dx = int(x) - int(x);
		int dy = int(y) - int(y);
		return (dx * dx + dy * dy) <= int(r * r);
	}

	bool BoundingCircle::Intersects(const BoundingCircle& circle) const
	{
		int dx = int(x) - int(circle.x);
		int dy = int(y) - int(circle.y);
		unsigned rSum = r + circle.r;

		return (dx * dx + dy * dy) <= int(rSum * rSum);
	}

	bool BoundingCircle::Intersects(const BoundingBox& box) const
	{
		return box.Intersects(*this);
	}

	bool BoundingCircle::Intersects(const BoundingArea& area) const
	{
		return area.Intersects(*this);
	}

	bool BoundingCircle::IntersectsLine(unsigned x1, unsigned y1, unsigned x2, unsigned y2) const
	{
		float dx = float(x2) - float(x1);
		float dy = float(y2) - float(y1);

		float fx = float(x1) - float(x);
		float fy = float(y1) - float(y);

		float a = dx * dx + dy * dy;
		float b = 2.0f * (fx * dx + fy * dy);
		float c = (fx * fx + fy * fy) - float(r * r);

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0.0f)
			return false;

		discriminant = std::sqrt(discriminant);

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		return (t1 >= 0.0f && t1 <= 1.0f) ||
			(t2 >= 0.0f && t2 <= 1.0f);
	}

	BoundingCircle* BoundingCircle::Clone() const
	{
		return new BoundingCircle(*this);
	}
}
