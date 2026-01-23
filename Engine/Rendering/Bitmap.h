#pragma once

#include <map>
#include <string>
#include <cstdint>

#include "Utils/Common.h"
#include "Rendering/Color.h"

#include <functional>

namespace gfx
{
	typedef uint8_t*	PixelMemory;
	typedef void*		Bitmap;

	Bitmap	BitmapLoad(const char* path);
	Bitmap	BitmapCreate(Dim w, Dim h);
	Bitmap	BitmapCopy(Bitmap bmp);
	void	BitmapClear(Bitmap bmp, Color c);
	void	BitmapDestroy(Bitmap bmp);
	Dim		BitmapGetWidth(Bitmap bmp);
	Dim		BitmapGetHeight(Bitmap bmp);

	bool		BitmapLock(Bitmap bmp);
	void		BitmapUnlock(Bitmap bmp);
	PixelMemory	BitmapGetMemory(Bitmap bmp);
	int			BitmapGetLineOffset(Bitmap bmp);

	void	WritePixelColor(PixelMemory pixelmem, const RGBA& value);
	void	ReadPixelColor(PixelMemory pixelmem, RGBA* value); 
	Color   GetPixel(PixelMemory mem);
	void	PutPixel(Bitmap bmp, Dim x, Dim y, Color c);

	void BitmapBlit(
		Bitmap src, const Rect& from,
		Bitmap dest, const Point& to
	);

	void BitmapBlitScaled(
		Bitmap src, const Rect& from,
		Bitmap dest, const Rect& to
	);

	using BitmapAccessFunctor = std::function<bool(PixelMemory)>;
	void BitmapAccessPixels(Bitmap bmp, const BitmapAccessFunctor& func);

	class BitmapLoader
	{
	public:
		BitmapLoader(void) = default;
		~BitmapLoader() { CleanUp(); }

		Bitmap	Load(const std::string& path);
		void	CleanUp(void);

	private:
		Bitmap GetBitmap(const std::string& path) const;

	private:
		using Bitmaps = std::map<std::string, Bitmap>;

		Bitmaps	m_Bitmaps;
	};
}
