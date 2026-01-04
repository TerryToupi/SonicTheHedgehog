#include "Rendering/Bitmap.h"
#include "Utils/Assert.h"

#include <SDL3/SDL.h>
#include <Rendering/stb_image.h>

#include <cstdlib>
#include <cstring>

namespace gfx
{
	extern SDL_Window*		g_pWindow;
	extern SDL_Renderer*	g_pRenderer;

	static Color			g_Colorkey;

	struct BitmapData
	{
		// CPU
		Dim			width = 0;
		Dim			height = 0;
		Dim			channels = 4;
		PixelMemory pixels = nullptr;

		// GPU
		SDL_Texture*	texture = nullptr;
		void*			gpuPixels = nullptr;
		int				pitch = 0;
	};

	Bitmap BitmapLoad(const char* path)
	{
		auto data = (BitmapData*)std::malloc(sizeof(BitmapData));
		ASSERT(data, "Failed to allocate memory for bitmap");

		int w = 0, h = 0, channels = 0;
		PixelMemory pixels = stbi_load(
			path,
			&w,
			&h,
			&channels,
			STBI_rgb_alpha
		);
		ASSERT((channels == sizeof(Color)), "Failed. Not supported format in use!");

		size_t textureByteSize = w * h * channels;

		data->width = w;
		data->height = h;
		data->channels = channels;
		data->pixels = pixels;

		data->texture = SDL_CreateTexture( 
			g_pRenderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			data->width, data->height
		);
		ASSERT(data->texture, SDL_GetError());

		ASSERT((data->gpuPixels == nullptr), "A Texture can not be locked twice!");
		if (!SDL_LockTexture(data->texture, nullptr, &data->gpuPixels, &data->pitch))
			ASSERT(false, SDL_GetError());

		for (int y = 0; y < h; ++y)
			std::memcpy(
				(PixelMemory)data->gpuPixels + y * data->pitch,
				(PixelMemory)data->pixels + y * data->width * data->channels,
				data->width * data->channels
			);

		SDL_UnlockTexture(data->texture);
		data->gpuPixels = nullptr;

		return (Bitmap)data;
	}

	Bitmap BitmapCreate(Dim w, Dim h)
	{
		auto data = (BitmapData*)std::malloc(sizeof(BitmapData));
		ASSERT(data, "Failed to allocate memory for bitmap");

		size_t textureByteSize = w * h * sizeof(Color);

		data->width = w;
		data->height = h;
		data->channels = sizeof(Color);

		data->pixels = (PixelMemory)std::malloc(textureByteSize);
		ASSERT(data->pixels, "Failed to allocate memory for bitmap pixels");
		
		std::memset(data->pixels, 0, textureByteSize);

		data->texture = SDL_CreateTexture( 
			g_pRenderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			data->width, data->height
		);
		ASSERT(data->texture, SDL_GetError());

		ASSERT((data->gpuPixels == nullptr), "A Texture can not be locked twice!");
		if (!SDL_LockTexture(data->texture, nullptr, &data->gpuPixels, &data->pitch))
			ASSERT(false, SDL_GetError());

		for (int y = 0; y < h; ++y)
			std::memcpy(
				(PixelMemory)data->gpuPixels + y * data->pitch,
				(PixelMemory)data->pixels + y * data->width * data->channels,
				data->width * data->channels
			);

		SDL_UnlockTexture(data->texture);
		data->gpuPixels = nullptr;

		return (Bitmap)data;
	}

	Bitmap BitmapCopy(Bitmap bmp)
	{
		ASSERT(bmp, "Given bitmap to cpy was NULL!");
		auto data = (BitmapData*)(bmp);
		
		size_t textureByteSize = data->width * data->height * data->channels;

		auto cpy = (BitmapData*)std::malloc(sizeof(BitmapData));
		ASSERT(cpy, "Failed to malloc for cpy BitmapData");

		cpy->width = data->width;
		cpy->height = data->height;
		cpy->channels = data->channels;

		cpy->pixels = (PixelMemory)std::malloc(textureByteSize);
		ASSERT(cpy->pixels, "Failed to malloc for cpy BitmapData pixels");

		std::memcpy(
			(PixelMemory)cpy->pixels,
			(PixelMemory)data->pixels,
			textureByteSize
		);

		cpy->texture = SDL_CreateTexture( 
			g_pRenderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			cpy->width, cpy->height
		);
		ASSERT(cpy->texture, SDL_GetError());

		ASSERT((cpy->gpuPixels == nullptr), "A Texture can not be locked twice!");
		if (!SDL_LockTexture(cpy->texture, nullptr, &cpy->gpuPixels, &cpy->pitch))
			ASSERT(false, SDL_GetError());

		for (int y = 0; y < cpy->height; ++y)
			std::memcpy(
				(PixelMemory)cpy->gpuPixels + y * cpy->pitch,
				(PixelMemory)cpy->pixels + y * cpy->width * cpy->channels,
				cpy->width * cpy->channels
			);

		SDL_UnlockTexture(cpy->texture);
		cpy->gpuPixels = nullptr;

		return (Bitmap)cpy;
	}

	void BitmapClear(Bitmap bmp, Color c)
	{
		ASSERT(bmp, "Failed Bitmap was NULL");
		auto data = (BitmapData*)(bmp);

		size_t textureByteSize = data->width * data->height * data->channels;
		
		std::memset(
			(PixelMemory)data->pixels,
			(int)c,
			textureByteSize
		);

		ASSERT((data->gpuPixels == nullptr), "A Texture can not be locked twice!");
		if (!SDL_LockTexture(data->texture, nullptr, &data->gpuPixels, &data->pitch))
			ASSERT(false, SDL_GetError());

		for (int y = 0; y < data->height; ++y)
			std::memcpy(
				(PixelMemory)data->gpuPixels + y * data->pitch,
				(PixelMemory)data->pixels + y * data->width * data->channels,
				data->width * data->channels
			);

		SDL_UnlockTexture(data->texture);
		data->gpuPixels= nullptr;
	}

	void BitmapDestroy(Bitmap bmp)
	{
		ASSERT(bmp, "Failed bitmap was NULL!");
		auto data = (BitmapData*)(bmp);

		ASSERT((data->gpuPixels == nullptr), "Can't free a locked texture!");

		free(data->pixels);
		SDL_DestroyTexture(data->texture);
		free(data);
	}

	Bitmap BitmapGetScreen(void)
	{
		return nullptr;
	}

	Dim BitmapGetWidth(Bitmap bmp)
	{
		ASSERT(bmp, "Failed bitmap was NULL!");
		auto data = (BitmapData*)(bmp);
		return data->width;
	}

	Dim BitmapGetHeight(Bitmap bmp)
	{
		ASSERT(bmp, "Failed bitmap was NULL!");
		auto data = (BitmapData*)(bmp);
		return data->height;
	}

	bool BitmapLock(Bitmap bmp)
	{
		ASSERT(bmp, "Failed, bitmap was nullptr");
		auto data = (BitmapData*)(bmp);

		ASSERT((data->gpuPixels == nullptr), "A Texture can not be locked twice!");
		bool result = SDL_LockTexture(data->texture, nullptr, &data->gpuPixels, &data->pitch); 
		ASSERT(result, SDL_GetError());
		return result;
	}

	void BitmapUnlock(Bitmap bmp)
	{
		ASSERT(bmp, "Failed, bitmap was nullptr");
		auto data = (BitmapData*)(bmp);

		ASSERT((data->gpuPixels != nullptr), "A Texture myst be locked first!");
		SDL_UnlockTexture(data->texture);
		ASSERT((data->gpuPixels == nullptr), "Failed to unlock the texture!");
		data->gpuPixels = nullptr;
	}

	PixelMemory BitmapGetMemory(Bitmap bmp)
	{
		ASSERT(bmp, "Failed, bitmap was nullptr");
		auto data = (BitmapData*)(bmp);
		
		ASSERT(data->pixels, "Pixel memory was nullptr!");
		return data->pixels;
	}

	int BitmapGetLineOffset(Bitmap bmp)
	{
		ASSERT(bmp, "Failed, bitmap was nullptr");
		auto data = (BitmapData*)(bmp);
		
		return data->channels;
	}

	void WritePixelColor(PixelMemory pixelmem, const RGBA& value)
	{
		ASSERT(pixelmem, "Failed, pixel memory is nullptr");
		Color c = MakeColor(value.r, value.g, value.b, value.a);
		std::memcpy(pixelmem, &c, sizeof(Color));
	}

	void ReadPixelColor(PixelMemory pixelmem, RGBA* value)
	{
		ASSERT(pixelmem, "Failed, pixel memroy is nullptr");
		RGBValue r = GetRedRGBA((uint8_t*)pixelmem);
		RGBValue g = GetGreenRGBA((uint8_t*)pixelmem);
		RGBValue b = GetBlueRGBA((uint8_t*)pixelmem);
		RGBValue a = GetAlphaRGBA((uint8_t*)pixelmem);

		ASSERT(value, "Failed, value memory was nullptr!");
		(*value).r = r;
		(*value).g = g;
		(*value).b = b;
		(*value).a = a;
	}

	void PutPixel(Bitmap bmp, Dim x, Dim y, Color c)
	{ 
		if (!BitmapLock(bmp))
			return;

		RGBA rgba;
		rgba.r = GetRedRGBA((PixelMemory)(&c));
		rgba.g = GetGreenRGBA((PixelMemory)(&c));
		rgba.b = GetBlueRGBA((PixelMemory)(&c));
		rgba.a = GetAlphaRGBA((PixelMemory)(&c));

		WritePixelColor(
			BitmapGetMemory(bmp) + (y * (BitmapGetWidth(bmp) * BitmapGetLineOffset(bmp)) + x),
			rgba
		);

		BitmapUnlock(bmp);
	}

	void SetColorKey(Color c)
	{
		g_Colorkey = c;
	}

	Color GetColorKey(void)
	{
		return g_Colorkey;
	}

	Bitmap BitmapLoader::Load(std::string path) 
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

