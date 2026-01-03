#pragma once

#include <cstdint>

#include "Utils/Common.h"
#include "Rendering/Color.h"

namespace gfx
{
	typedef uint8_t*	PixelMemory;
	typedef void*		Bitmap;

	Bitmap	BitmapLoad(const char* path);
	Bitmap	BitmapCreate(Dim w, Dim h);
	Bitmap	BitmapCopy(Bitmap bmp);
	void	BitmapClear(Bitmap bmp, Color c);
	void	BitmapDestroy(Bitmap bmp);
	Bitmap	BitmapGetScreen(void);
	Dim		BitmapGetWidth(Bitmap bmp);
	Dim		BitmapGetHeight(Bitmap bmp);
	void	BitmapBlit(
		Bitmap src, const Rect& from,
		Bitmap dest, const Point& to
	);

	bool		BitmapLock(Bitmap bitmap);
	void		BitmapUnlock(Bitmap bitmap);
	PixelMemory	BitmapGetMemory(Bitmap bitmap);
	int			BitmapGetLineOffset(Bitmap bitmap);

	void		WritePixelColor(PixelMemory pixelmem, const RGBA& value);
	void		ReadPixelColor(PixelMemory pixelmem, RGBA* value);
}
