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

	struct BitmapData
	{
		SDL_Surface* surf = nullptr;
		SDL_Texture* texture = nullptr;
		int isDirty = 0;
	};
}

namespace gfx
{
	static inline BitmapData* AllocateBitmapData()
	{
		return (BitmapData*)malloc(sizeof(BitmapData));
	}

	Bitmap BitmapLoad(const char* path)
	{
		constexpr int STBI_BPP = 4;

		int w = 0, h = 0, ch = 0;
		uint8_t* data = stbi_load(path, &w, &h, &ch, STBI_rgb_alpha);
		ASSERT(data, "STB_image failed to load texture!");

		SDL_Surface* surf = SDL_CreateSurface(
			w,
			h,
			g_pSuportedPixelFormat->format
		);
		ASSERT(surf, SDL_GetError());

		ASSERT(SDL_SetSurfaceBlendMode(
			surf, 
			SDL_BLENDMODE_BLEND
		), SDL_GetError());

		ASSERT(SDL_LockSurface(surf), SDL_GetError());
		{
			auto dst = (uint8_t*)surf->pixels;
			int dstPitch = surf->pitch;

			const auto src = (uint8_t*)data;
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

		SDL_Texture* texture = SDL_CreateTexture(
			g_pRenderer, 
			g_pSuportedPixelFormat->format,
			SDL_TEXTUREACCESS_TARGET,
			surf->w,
			surf->h
		);
		ASSERT(texture, SDL_GetError());

		ASSERT(SDL_UpdateTexture(
			texture,
			nullptr,
			surf->pixels,
			surf->pitch
		), SDL_GetError());

		ASSERT(SDL_SetTextureBlendMode(
			texture,
			SDL_BLENDMODE_BLEND
		), SDL_GetError());

		ASSERT(SDL_SetTextureScaleMode(
			texture,
			SDL_SCALEMODE_PIXELART
		), SDL_GetError());

		BitmapData* bitmap = AllocateBitmapData();
		bitmap->surf = surf;
		bitmap->texture = texture;
		bitmap->isDirty = 0;

		return (Bitmap)bitmap;
	}

	Bitmap BitmapCreate(Dim w, Dim h)
	{
		SDL_Surface* surf = SDL_CreateSurface(
			(int)w,
			(int)h,
			g_pSuportedPixelFormat->format	
		);
		ASSERT(surf, SDL_GetError());

		ASSERT(SDL_SetSurfaceBlendMode(
			surf, 
			SDL_BLENDMODE_BLEND
		), SDL_GetError());

		ASSERT(SDL_FillSurfaceRect(
			surf, 
			nullptr, 
			g_ClearColor
		), SDL_GetError());

		SDL_Texture* texture = SDL_CreateTexture(
			g_pRenderer,
			g_pSuportedPixelFormat->format,
			SDL_TEXTUREACCESS_TARGET,
			surf->w,
			surf->h
		);
		ASSERT(texture, SDL_GetError());

		ASSERT(SDL_UpdateTexture(
			texture,
			nullptr,
			surf->pixels,
			surf->pitch
		), SDL_GetError());

		ASSERT(SDL_SetTextureBlendMode(
			texture,
			SDL_BLENDMODE_BLEND
		), SDL_GetError());

		ASSERT(SDL_SetTextureScaleMode(
			texture,
			SDL_SCALEMODE_PIXELART
		), SDL_GetError());

		BitmapData* bitmap = AllocateBitmapData();
		bitmap->surf = surf;
		bitmap->texture = texture;
		bitmap->isDirty = 0;

		return (Bitmap)bitmap;
	}

	Bitmap BitmapCopy(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;

		auto cpySurf = SDL_ConvertSurface(bmpSurf, bmpSurf->format);
		ASSERT(cpySurf, SDL_GetError());

		ASSERT(SDL_SetSurfaceBlendMode(
			cpySurf, 
			SDL_BLENDMODE_BLEND
		), SDL_GetError());

		SDL_Texture* cpyTexture = SDL_CreateTexture(
			g_pRenderer,
			g_pSuportedPixelFormat->format,
			SDL_TEXTUREACCESS_TARGET,
			cpySurf->w,
			cpySurf->h
		);
		ASSERT(cpyTexture, SDL_GetError());

		ASSERT(SDL_UpdateTexture(
			cpyTexture,
			nullptr,
			cpySurf->pixels,
			cpySurf->pitch
		), SDL_GetError());

		ASSERT(SDL_SetTextureBlendMode(
			cpyTexture,
			SDL_BLENDMODE_BLEND
		), SDL_GetError());

		ASSERT(SDL_SetTextureScaleMode(
			cpyTexture,
			SDL_SCALEMODE_PIXELART
		), SDL_GetError());

		BitmapData* bitmap = AllocateBitmapData();
		bitmap->surf = cpySurf;
		bitmap->texture = cpyTexture;
		bitmap->isDirty = 0;

		return (Bitmap)bitmap;
	}

	void BitmapClear(Bitmap bmp, Color c)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;
		auto bmpTexture = bmpData->texture;

		ASSERT(SDL_FillSurfaceRect(
			bmpSurf,
			nullptr,
			(Uint32)c
		), SDL_GetError());

		ASSERT(SDL_UpdateTexture(
			bmpTexture,
			nullptr,
			bmpSurf->pixels,
			bmpSurf->pitch
		), SDL_GetError());
	}

	void BitmapDestroy(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;
		auto bmpTexture = bmpData->texture;

		SDL_DestroySurface(bmpSurf);
		SDL_DestroyTexture(bmpTexture);
		free(bmp);
	}

	Dim BitmapGetWidth(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;

		return (Dim)bmpSurf->w;
	}

	Dim BitmapGetHeight(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;

		return (Dim)bmpSurf->h;
	}

	bool BitmapLock(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;
		auto bmpText = bmpData->texture;
	
		if (bmpData->isDirty)
		{
			ASSERT(SDL_SetRenderTarget(
				g_pRenderer,
				bmpText
			), SDL_GetError());
		
			SDL_DestroySurface(bmpData->surf);
			bmpData->surf = SDL_RenderReadPixels(g_pRenderer, nullptr);
			ASSERT(bmpData->surf, "FAILED to read texture from gpu and update the gpu surface!");

			if (bmpData->surf->format != g_pSuportedPixelFormat->format)
			{
				auto converted = SDL_ConvertSurface(bmpData->surf, g_pSuportedPixelFormat->format);
				ASSERT(converted, "FAILED to convert to supported pixel formal!");
				SDL_DestroySurface(bmpData->surf);
				bmpData->surf = converted;
			}

			ASSERT(SDL_SetRenderTarget(
				g_pRenderer,
				nullptr
			), SDL_GetError());

			bmpData->isDirty = 0;
		}

		if (SDL_MUSTLOCK(bmpSurf))
			if (!SDL_LockSurface(bmpSurf))
			{
				ASSERT(false, SDL_GetError());
				return false;
			}


		return true;
	}

	void BitmapUnlock(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;
		auto bmpTexture = bmpData->texture;

		SDL_UnlockSurface(bmpSurf);

		ASSERT(SDL_UpdateTexture(
			bmpTexture,
			nullptr,
			bmpSurf->pixels,
			bmpSurf->pitch
		), SDL_GetError());
	}

	PixelMemory BitmapGetMemory(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;

		return (PixelMemory)bmpSurf->pixels;
	}

	int BitmapGetLineOffset(Bitmap bmp)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");
		auto bmpData = (BitmapData*)(bmp);
		auto bmpSurf = bmpData->surf;

		return bmpSurf->pitch;
	}

	void WritePixelColor(PixelMemory pixelmem, const RGBA& value)
	{
		ASSERT(pixelmem, "Failed. Pixel memory was nullptr!");

		Color c = MakeColor(value.r, value.g, value.b, value.a);
		memcpy(pixelmem, &c, sizeof(Color));
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

	gfx::Color GetPixel(PixelMemory mem)
	{
		RGBA c;
		ReadPixelColor(mem, &c);
		return MakeColor(c.r, c.g, c.b, c.a);
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

	void BitmapSetColorKey(Bitmap bmp, RGBValue r, RGBValue g, RGBValue b)
	{
		ASSERT(bmp, "Failed. Bitmap was nullptr!");

		if (!BitmapLock(bmp))
			return;

		uint8_t* base = BitmapGetMemory(bmp);
		int pitch = BitmapGetLineOffset(bmp);
		int w = (int)BitmapGetWidth(bmp);
		int h = (int)BitmapGetHeight(bmp);

		Color keyColor = MakeColor(r, g, b, 255);
		Color transparent = MakeColor(r, g, b, 0);

		for (int y = 0; y < h; ++y)
		{
			Color* row = reinterpret_cast<Color*>(base + y * pitch);
			for (int x = 0; x < w; ++x)
			{
				if (row[x] == keyColor)
					row[x] = transparent;
			}
		}

		BitmapUnlock(bmp);
	}

	void BitmapBlit(Bitmap src, const Rect& from, Bitmap dest, const Point& to)
	{
		ASSERT(src, "Failed. Bitmap was nullptr!");
		auto srcData = (BitmapData*)(src);
		auto srcTexture = srcData->texture;

		ASSERT(dest, "Failed. Dest bitmap was nullptr!");
		auto destData = (BitmapData*)(dest);
		auto destTexture = destData->texture;

		SDL_FRect srcRect{ (float)from.x, (float)from.y, (float)from.w, (float)from.h };
		SDL_FRect dstRect{ (float)to.x,   (float)to.y,   (float)from.w, (float)from.h };

		ASSERT(SDL_SetRenderTarget(
			g_pRenderer,
			destTexture
		), SDL_GetError());

		ASSERT(SDL_RenderTexture(
			g_pRenderer,
			srcTexture,
			&srcRect,
			&dstRect
		), SDL_GetError());

		ASSERT(SDL_SetRenderTarget(
			g_pRenderer,
			nullptr
		), SDL_GetError());

		destData->isDirty = 1;
	}

	void BitmapBlitScaled(Bitmap src, const Rect& from, Bitmap dest, const Rect& to)
	{
		ASSERT(src, "Failed. Bitmap was nullptr!");
		auto srcData = (BitmapData*)(src);
		auto srcTexture = srcData->texture;

		ASSERT(dest, "Failed. Dest bitmap was nullptr!");
		auto destData = (BitmapData*)(dest);
		auto destTexture = destData->texture;

		SDL_FRect srcRect{ (float)from.x, (float)from.y, (float)from.w, (float)from.h };
		SDL_FRect dstRect{ (float)to.x,   (float)to.y,   (float)to.w,   (float)to.h };

		ASSERT(SDL_SetRenderTarget(
			g_pRenderer,
			destTexture
		), SDL_GetError());

		ASSERT(SDL_RenderTexture(
			g_pRenderer,
			srcTexture,
			&srcRect,
			&dstRect
		), SDL_GetError());

		ASSERT(SDL_SetRenderTarget(
			g_pRenderer,
			nullptr
		), SDL_GetError());

		destData->isDirty = 1;
	}

	void BitmapAccessPixels(Bitmap bmp, const BitmapAccessFunctor& func)
	{
		ASSERT(BitmapLock(bmp), "FAILED. BitmapAccessPixels failed to lock bitmap!");

		auto mem = BitmapGetMemory(bmp);
		auto offset = BitmapGetLineOffset(bmp);
		auto bpp = g_pSuportedPixelFormat->bytes_per_pixel;

		for (auto y = BitmapGetHeight(bmp); y--;)
		{
			auto buff = mem;
			for (auto x = BitmapGetWidth(bmp); x--;)
			{
				func(buff);
				buff += bpp;
			}
			mem += offset;
		}

		BitmapUnlock(bmp);
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

