#include "Rendering/Renderer.h"
#include "Rendering/Color.h"
#include "Utils/Assert.h"

#include <SDL3/SDL.h>

namespace gfx
{
	SDL_Window*					  g_pWindow = nullptr;
	SDL_Renderer*				  g_pRenderer = nullptr;
	const SDL_PixelFormatDetails* g_pSuportedPixelFormat = nullptr;
	Color						  g_ClearColor;

	struct BitmapData
	{
		SDL_Surface* surf = nullptr;
		SDL_Texture* texture = nullptr;
	};

	struct ViewData
	{
		bool   dpyChanged = false;
		Dim	   dpyX = 0, dpyY = 0;
		Dim	   bufX = 0, bufY = 0;
		Bitmap buffer = nullptr;
	} g_ViewData;
}

namespace gfx
{
	void Open(const char* title, Dim rw, Dim rh)
	{
		ASSERT((g_pWindow == nullptr), "Window already initialized!");
		ASSERT((g_pRenderer == nullptr), "Renderer already initilized!");
		
		SDL_WindowFlags flags = { 0 };
		flags |= SDL_WINDOW_INPUT_FOCUS;
		flags |= SDL_WINDOW_MOUSE_CAPTURE;
		flags |= SDL_WINDOW_RESIZABLE;

		if (!SDL_CreateWindowAndRenderer(title, (int)rw, (int)rh, flags, &g_pWindow, &g_pRenderer))
			SDL_Log("[SDL] Failed to initialize Window: %s", SDL_GetError());

		ASSERT((g_pWindow), "Failed to initilize SDL window!");
		ASSERT((g_pRenderer), "Failed to initilize SDL renderer!");

		g_pSuportedPixelFormat = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
		ASSERT((g_pSuportedPixelFormat), SDL_GetError());

		g_ClearColor = MakeColor(255, 255, 255, 255);

		g_ViewData.dpyX = rw;
		g_ViewData.dpyY = rh;
	}

	void Close(void)
	{
		ASSERT((g_pWindow), "SDL window has already been destroyed!");
		ASSERT((g_pRenderer), "SDL renderer has already been destroyed!");
		ASSERT((g_pSuportedPixelFormat), "SDL supported pixel format settings has already been destroyed!");
		ASSERT((g_ViewData.buffer), "Display buffer has been destroyed!");

		BitmapDestroy(g_ViewData.buffer);
		SDL_DestroyRenderer(g_pRenderer);
		SDL_DestroyWindow(g_pWindow);
	}

	Dim GetResWidth(void)
	{
		ASSERT(!(g_ViewData.dpyChanged), "Window data has beed changed!");
		return g_ViewData.dpyX;
	}

	Dim GetResHeight(void)
	{
		ASSERT(!(g_ViewData.dpyChanged), "Window data has beed changed!");
		return g_ViewData.dpyY;
	}

	Color GetBackgroundColor(void)
	{
		return g_ClearColor;
	}

	void SetBackgroundColor(Color c)
	{
		g_ClearColor = c;
	}

	void SetScreenBuffer(Dim x, Dim y)
	{
		if (g_ViewData.buffer)
			BitmapDestroy(g_ViewData.buffer);

		g_ViewData.buffer = BitmapCreate(x, y);
		g_ViewData.bufX = x;
		g_ViewData.bufY = y;
		ASSERT(g_ViewData.buffer, SDL_GetError());
	}

	Bitmap GetScreenBuffer(void)
	{
		ASSERT((g_ViewData.buffer), "Failed. Display buffer has not been initialized or has been destroyed!");
		return g_ViewData.buffer;
	}

	Rect GetScreenRect(void)
	{
		return { 0, 0, g_ViewData.bufX, g_ViewData.bufY };
	}

	void RaiseWindowResizeEvent(void)
	{
		g_ViewData.dpyChanged = true;
	}

	static inline void ResizeWindow(void)
	{
		if (g_ViewData.dpyChanged)
		{
			ASSERT((g_pWindow), "Window has been destroyed!");

			ASSERT(SDL_GetWindowSize(
				g_pWindow, 
				(int*)(&g_ViewData.dpyX), 
				(int*)(&g_ViewData.dpyY)
			), SDL_GetError());

			g_ViewData.dpyChanged = false;
		}
	}

	void Flush()
	{
		ASSERT((g_ViewData.buffer), "Display buffer bitmap has been destroyed!");
		auto bufferData = (BitmapData*)(g_ViewData.buffer);
		auto bufferTexture = bufferData->texture;

		ASSERT(SDL_SetRenderTarget(
			g_pRenderer, 
			nullptr
		), SDL_GetError());

		ASSERT(SDL_RenderClear(
			g_pRenderer
		), SDL_GetError());

		ASSERT(SDL_RenderTexture(
			g_pRenderer, 
			bufferTexture, 
			nullptr, 
			nullptr
		), SDL_GetError());

		ASSERT(SDL_RenderPresent(
			g_pRenderer
		), SDL_GetError());
	}
}