#pragma once

#include <cstdint>

typedef uint8_t  byte;
typedef uint16_t Dim;

typedef uint64_t Time;
typedef uint64_t TimeStamp;

typedef uint32_t Index;
typedef uint16_t GridIndex;

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

template <typename Tnum>
int number_sign(Tnum x)
{
	return x > 0 ? 1 : x < 0 ? -1
		: 0;
}
