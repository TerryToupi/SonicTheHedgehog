#include "Rendering/Bitmap.h"
#include "Utils/Assert.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>

namespace gfx
{
	extern SDL_Window*		g_pWindow;
	extern SDL_Renderer*	g_pRenderer;

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
		data->gpuPixels= nullptr;

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

	void BitmapBlit(Bitmap src, const Rect& from, Bitmap dest, const Point& to)
	{

	}

	bool BitmapLock(Bitmap bitmap)
	{

	}

	void BitmapUnlock(Bitmap bitmap)
	{

	}

	PixelMemory BitmapGetMemory(Bitmap bitmap)
	{

	}

	int BitmapGetLineOffset(Bitmap bitmap)
	{

	}

	void WritePixelColor(PixelMemory pixelmem, const RGBA& value)
	{

	}

	void ReadPixelColor(PixelMemory pixelmem, RGBA* value)
	{

	}
}


