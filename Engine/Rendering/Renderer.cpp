#include "Rendering/Renderer.h"
#include "Utils/Assert.h"

#include <SDL3/SDL.h>

SDL_Window*		g_pWindow = nullptr;
SDL_Renderer*	g_pRenderer = nullptr;

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
	}

	void Close(void)
	{
		ASSERT((g_pWindow), "SDL window has already been destroyed!");
		ASSERT((g_pRenderer), "SDL renderer has already been destroyed!");

		SDL_DestroyRenderer(g_pRenderer);
		SDL_DestroyWindow(g_pWindow);
	}

	Dim GetResWidth(void)
	{
		ASSERT((g_pWindow), "No window width because SDL window has been destroyed!");

		return Dim();
	}

	Dim GetResHeight(void)
	{
		ASSERT((g_pWindow), "No window height because SDL window has been destroyed!");

		return Dim();
	}
}