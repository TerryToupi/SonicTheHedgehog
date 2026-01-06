#pragma once

#include <cstdint>

typedef unsigned char  byte;
typedef unsigned short Dim;

typedef uint64_t Time;
typedef uint64_t TimeSamp;

struct Rect
{
	int x = 0, y = 0, w = 0, h = 0;

	bool operator==(const Rect& r) const
	{
		return r.x == x &&
			r.y == y &&
			r.w == w &&
			r.h == h;
	}
};

struct Point
{
	int x = 0;
	int y = 0;
};
