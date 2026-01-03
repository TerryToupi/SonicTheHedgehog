#include "Rendering/Color.h"

#define RGBA_R_SHIFT 24
#define RGBA_G_SHIFT 16
#define RGBA_B_SHIFT 8
#define RGBA_A_SHIFT 0

#define RGBA_R_MASK (0xFFu << RGBA_R_SHIFT)
#define RGBA_G_MASK (0xFFu << RGBA_G_SHIFT)
#define RGBA_B_MASK (0xFFu << RGBA_B_SHIFT)
#define RGBA_A_MASK (0xFFu << RGBA_A_SHIFT)

#define RGBA8888(r, g, b, a) \
    ( ((uint32_t)(r) << RGBA_R_SHIFT) | \
      ((uint32_t)(g) << RGBA_G_SHIFT) | \
      ((uint32_t)(b) << RGBA_B_SHIFT) | \
      ((uint32_t)(a) << RGBA_A_SHIFT) )

#define RGBA_GET_R(c) (((c) & RGBA_R_MASK) >> RGBA_R_SHIFT)
#define RGBA_GET_G(c) (((c) & RGBA_G_MASK) >> RGBA_G_SHIFT)
#define RGBA_GET_B(c) (((c) & RGBA_B_MASK) >> RGBA_B_SHIFT)
#define RGBA_GET_A(c) (((c) & RGBA_A_MASK) >> RGBA_A_SHIFT)

namespace gfx
{
	Color MakeColor(RGBValue r, RGBValue g, RGBValue b, RGBValue a /*= 0*/)
	{
        return (Color)(RGBA8888(r, g, b, a));
	}

	unsigned GetRedShiftRGBA(void)
	{
		return (unsigned)RGBA_R_SHIFT;
	}

	unsigned GetRedBitMaskRGBA(void)
	{
		return (unsigned)RGBA_R_MASK;
	}

	unsigned GetGreenShiftRGBA(void)
	{
		return (unsigned)RGBA_G_SHIFT;
	}

	unsigned GetGreenBitMaskRGBA(void)
	{
		return (unsigned)RGBA_G_MASK;
	}

	unsigned GetBlueShiftRGBA(void)
	{
		return (unsigned)RGBA_B_SHIFT;
	}

	unsigned GetBlueBitMaskRGBA(void)
	{
		return (unsigned)RGBA_B_MASK;
	}

	unsigned GetAlphaShiftRGBA(void)
	{
		return (unsigned)RGBA_A_SHIFT;
	}

	unsigned GetAlphaBitMaskRGBA(void)
	{
		return (unsigned)RGBA_A_MASK;
	}

	RGBValue GetRedRGBA(uint8_t* pixel)
	{
		Color c = *((Color*)pixel);
		return (c & GetRedBitMaskRGBA()) >> GetRedShiftRGBA();
	}

	RGBValue GetGreenRGBA(uint8_t* pixel)
	{
		Color c = *((Color*)pixel);
		return (c & GetGreenBitMaskRGBA()) >> GetGreenShiftRGBA();
	}

	RGBValue GetBlueRGBA(uint8_t* pixel)
	{
		Color c = *((Color*)pixel);
		return (c & GetBlueBitMaskRGBA()) >> GetBlueShiftRGBA();
	}

	RGBValue GetAlphaRGBA(uint8_t* pixel)
	{
		Color c = *((Color*)pixel);
		return (c & GetAlphaBitMaskRGBA()) >> GetAlphaShiftRGBA();
	}
}