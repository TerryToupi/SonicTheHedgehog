#include "Rendering/Color.h"
#include "Utils/Assert.h"

#include <SDL3/SDL.h>

namespace gfx 
{
	extern SDL_Window*					  g_pWindow;
	extern SDL_Renderer*				  g_pRenderer;
	extern const SDL_PixelFormatDetails*  g_pSuportedPixelFormat;
	extern Color						  g_ClearColor;
}

namespace gfx
{
	Color MakeColor(RGBValue r, RGBValue g, RGBValue b, RGBValue a /*= 255 */)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		Color c = 0;

		c |= ((Color)r << fmt->Rshift) & fmt->Rmask;
		c |= ((Color)g << fmt->Gshift) & fmt->Gmask;
		c |= ((Color)b << fmt->Bshift) & fmt->Bmask;
		c |= ((Color)a << fmt->Ashift) & fmt->Amask;

		return c;
	}

	unsigned GetRedShiftRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Rshift;
	}

	unsigned GetRedBitMaskRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Rmask;
	}

	unsigned GetGreenShiftRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Gshift;
	}

	unsigned GetGreenBitMaskRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Gmask;
	}

	unsigned GetBlueShiftRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Bshift;
	}

	unsigned GetBlueBitMaskRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Bmask;
	}

	unsigned GetAlphaShiftRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Ashift;
	}

	unsigned GetAlphaBitMaskRGBA(void)
	{
		const SDL_PixelFormatDetails* fmt = g_pSuportedPixelFormat;
		ASSERT(fmt, "Faild, support format was not found!");

		return (unsigned)fmt->Amask;
	}
}