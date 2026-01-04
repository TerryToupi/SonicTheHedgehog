#pragma once

#include <cstdint>

#include "Utils/Common.h"

namespace gfx
{
	typedef uint32_t	Color;
	typedef uint8_t		RGBValue;
	typedef uint8_t		Alpha;

	struct RGB
	{
		RGBValue r, g, b;
	};

	struct RGBA : public RGB
	{
		RGBValue a;
	};

	typedef RGB Palete[256];

	Color MakeColor (RGBValue r, RGBValue g, RGBValue b, RGBValue a = 0);

	unsigned GetRedShiftRGBA(void);
	unsigned GetRedBitMaskRGBA(void);
	unsigned GetGreenShiftRGBA(void);
	unsigned GetGreenBitMaskRGBA(void);
	unsigned GetBlueShiftRGBA(void);
	unsigned GetBlueBitMaskRGBA(void);
	unsigned GetAlphaShiftRGBA(void);
	unsigned GetAlphaBitMaskRGBA(void);
}