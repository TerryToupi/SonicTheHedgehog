#include "Rendering/Bitmap.h"
#include "Utils/Assert.h"

#include <SDL3/SDL.h>
#include <Rendering/stb_image.h>

#include <cstdlib>
#include <cstring>

namespace gfx 
{
	extern SDL_Window*					  g_pWindow;
	extern SDL_Renderer*				  g_pRenderer;
	extern const SDL_PixelFormatDetails*  g_pSuportedPixelFormat;
	extern Color						  g_ClearColor;
	extern Color						  g_ColorKey;
}

namespace gfx
{
	Bitmap BitmapLoad(const char* path)
	{
		constexpr int STBI_BPP = 4;

		int w = 0, h = 0, ch = 0;
		stbi_uc* data = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);
		ASSERT(data, "STB_image failed to load texture!");

		SDL_Surface* surf = SDL_CreateSurface(
			w,
			h,
			g_pSuportedPixelFormat->format
		);
		ASSERT(surf, SDL_GetError());
		ASSERT(SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND), SDL_GetError());

		SDL_LockSurface(surf);
		{
			auto dst = (uint8_t*)surf->pixels;
			int dstPitch = surf->pitch;

			const uint8_t* src = data;
			int srcPitch = w * STBI_BPP;

			for (int y = 0; y < h; y++)
			{
				auto dstRow = (Color*)(dst + y * dstPitch);
				const uint8_t* srcRow = src + y * srcPitch;

				for (int x = 0; x < w; x++)
				{
					const uint8_t* p = srcRow + x * STBI_BPP;
					dstRow[x] = MakeColor(p[0], p[1], p[2], p[3]);
				}
			}
		}
		SDL_UnlockSurface(surf);

		stbi_image_free(data);
		return (Bitmap)surf;
	}

	Bitmap BitmapCreate(Dim w, Dim h)
	{
		SDL_Surface* surf = SDL_CreateSurface(
			(int)w,
			(int)h,
			g_pSuportedPixelFormat->format	
		);
		ASSERT(surf, SDL_GetError());
		ASSERT(SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND), SDL_GetError());

		SDL_FillSurfaceRect(surf, nullptr, g_ClearColor);

		return (Bitmap)surf;
	}

	Bitmap BitmapCopy(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);
		auto cpy  = SDL_ConvertSurface(surf, surf->format);

		ASSERT(SDL_SetSurfaceBlendMode(cpy, SDL_BLENDMODE_BLEND), SDL_GetError());

		return (Bitmap)cpy;
	}

	void BitmapClear(Bitmap bmp, Color c)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);
		SDL_FillSurfaceRect(surf, nullptr, c);
	}

	void BitmapDestroy(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);
		SDL_DestroySurface(surf);
	}

	Dim BitmapGetWidth(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);
		return surf->w;
	}

	Dim BitmapGetHeight(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);
		return surf->h;
	}

	bool BitmapLock(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);
	
		if (SDL_MUSTLOCK(surf))
			if (!SDL_LockSurface(surf))
			{
				ASSERT(false, SDL_GetError());
				return false;
			}

		return true;
	}

	void BitmapUnlock(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);

		if (SDL_MUSTLOCK(surf))
			SDL_UnlockSurface(surf);
	}

	PixelMemory BitmapGetMemory(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);

		return (PixelMemory)surf->pixels;
	}

	int BitmapGetLineOffset(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto surf = (SDL_Surface*)(bmp);

		return surf->pitch;
	}

	void WritePixelColor(PixelMemory pixelmem, const RGBA& value)
	{
		ASSERT(pixelmem, "Failed. Pixel memory was nullptr!");

		Color c = MakeColor(value.r, value.g, value.b, value.a);
		std::memcpy(pixelmem, &c, sizeof(Color));
	}

	void ReadPixelColor(PixelMemory pixelmem, RGBA* value)
	{
		ASSERT(pixelmem, "Failed. Pixel memory was nullptr!");
		ASSERT(value, "Failed. Value memory was nullptr!");

		Color c = *(const Color*)pixelmem;
		value->r = (RGBValue)((c & GetRedBitMaskRGBA()) >> GetRedShiftRGBA());
		value->g = (RGBValue)((c & GetGreenBitMaskRGBA()) >> GetGreenShiftRGBA());
		value->b = (RGBValue)((c & GetBlueBitMaskRGBA()) >> GetBlueShiftRGBA());
		value->a = (RGBValue)((c & GetAlphaBitMaskRGBA()) >> GetAlphaShiftRGBA());
	}

	void PutPixel(Bitmap bmp, Dim x, Dim y, Color c)
	{ 
		ASSERT(bmp, "Failed. Bitmap was nullptr!");

		if (!BitmapLock(bmp))
			return;

		uint8_t* base = BitmapGetMemory(bmp);
		int pitch = BitmapGetLineOffset(bmp);

		*(Color*)(base + y * pitch + x * sizeof(Color)) = c;

		BitmapUnlock(bmp);
	}

	void SetColorKey(Color c)
	{
		g_ColorKey = c;
	}

	Color GetColorKey(void)
	{
		return g_ColorKey;
	}

	void BitmapBlit(Bitmap src, const Rect& from, Bitmap dest, const Point& to)
	{
		ASSERT(src, "Failed. Src bitmap was nullptr!");
		auto srcSurf = (SDL_Surface*)(src);

		ASSERT(dest, "Failed. Dest bitmap was nullptr!");
		auto destSurf = (SDL_Surface*)(dest);

		SDL_Rect srcRect{ from.x, from.y, from.w, from.h };
		SDL_Rect dstRect{ to.x,   to.y,   from.w, from.h };

		ASSERT(SDL_BlitSurface(
			srcSurf,
			&srcRect,
			destSurf,
			&dstRect
		), SDL_GetError());
	}

	void ScaledBlit(Bitmap src, const Rect& from, Bitmap dest, const Point& to)
	{
		ASSERT(src, "Failed. Src bitmap was nullptr!");
		auto srcSurf = (SDL_Surface*)(src);

		ASSERT(dest, "Failed. Dest bitmap was nullptr!");
		auto destSurf = (SDL_Surface*)(dest);

		SDL_Rect srcRect{ from.x, from.y, from.w, from.h };
		SDL_Rect dstRect{ to.x,   to.y,   from.w, from.h };

		ASSERT(SDL_BlitSurfaceScaled(
			srcSurf,
			&srcRect,
			destSurf,
			&dstRect,
			SDL_SCALEMODE_LINEAR
		), SDL_GetError());
	}

	void MaskedBitmapBlit(Bitmap src, const Rect& from, Bitmap dest, const Point& to)
	{
		ASSERT(src, "Failed. Src bitmap was nullptr!");
		auto srcSurf = (SDL_Surface*)(src);

		ASSERT(SDL_SetSurfaceColorKey(srcSurf, true, g_ColorKey), SDL_GetError());

		ASSERT(dest, "Failed. Dest bitmap was nullptr!");
		auto destSurf = (SDL_Surface*)(dest);

		ASSERT(SDL_SetSurfaceColorKey(destSurf, true, g_ColorKey), SDL_GetError());

		BitmapBlit(src, from, dest, to);

		ASSERT(SDL_SetSurfaceColorKey(srcSurf, false, g_ColorKey), SDL_GetError());
		ASSERT(SDL_SetSurfaceColorKey(destSurf, false, g_ColorKey), SDL_GetError());
	}

	void MaskedScaledBlit(Bitmap src, const Rect& from, Bitmap dest, const Point& to)
	{
		ASSERT(src, "Failed. Src bitmap was nullptr!");
		auto srcSurf = (SDL_Surface*)(src);

		ASSERT(SDL_SetSurfaceColorKey(srcSurf, true, g_ColorKey), SDL_GetError());

		ASSERT(dest, "Failed. Dest bitmap was nullptr!");
		auto destSurf = (SDL_Surface*)(dest);

		ASSERT(SDL_SetSurfaceColorKey(destSurf, true, g_ColorKey), SDL_GetError());

		ScaledBlit(src, from, dest, to);

		ASSERT(SDL_SetSurfaceColorKey(srcSurf, false, g_ColorKey), SDL_GetError());
		ASSERT(SDL_SetSurfaceColorKey(destSurf, false, g_ColorKey), SDL_GetError());
	}

	Bitmap BitmapLoader::Load(const std::string& path)
	{
		auto b = GetBitmap(path);
		if (!b)
		{
			b = BitmapLoad(path.c_str());
			ASSERT(b, "Failed. Bitmap was nullptr");
			m_Bitmaps[path] = b;
		}
		return b;
	}

	void BitmapLoader::CleanUp(void)
	{
		for (auto& i : m_Bitmaps)
			BitmapDestroy(i.second);
		m_Bitmaps.clear();
	}

	Bitmap BitmapLoader::GetBitmap(const std::string& path) const
	{
		auto i = m_Bitmaps.find(path);
		return i != m_Bitmaps.end() ? i->second : nullptr;
	}
}

