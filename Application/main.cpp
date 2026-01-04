#include <SDL3/SDL.h>
#include "Rendering/Renderer.h"

#include <string>

int main(void)
{
	bool close = false;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	gfx::Open("TOU KARAGIOZI TO SKAMNI", 1024, 720);

	gfx::BitmapLoader loader;
	gfx::Bitmap bmp = loader.Load(std::string(ASSETS) + "/Textures/a.png");

	while (!close)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
				close = true;
		}

		gfx::BitmapBlit(
			bmp,
			{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
			gfx::GetScreenBuffer(),
			{ 40, 40 }
		);

		gfx::BitmapBlit(
			bmp,
			{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
			gfx::GetScreenBuffer(),
			{ 500, 40 }
		);

		gfx::BitmapBlit(
			bmp,
			{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
			gfx::GetScreenBuffer(),
			{ 40, 500 }
		);

		gfx::BitmapBlit(
			bmp,
			{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
			gfx::GetScreenBuffer(),
			{ 500, 500 }
		);

		gfx::Flush();
	}

	gfx::Close();

	SDL_Quit();

	return 0;
}