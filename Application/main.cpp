#include <SDL3/SDL.h>
#include "Rendering/Renderer.h"
#include "Scene/SpriteManager.h"
#include "Scene/Sprite.h"

#include <string>

int main(void)
{
	bool close = false;

	gfx::Open("TOU KARAGIOZI TO SKAMNI", 1024, 720);
	gfx::SetScreenBuffer(1024, 1024);

	gfx::BitmapLoader loader;
	gfx::Bitmap bmp = loader.Load(std::string(ASSETS) + "/Textures/a.png");

	gfx::Bitmap cpy1 = gfx::BitmapCopy(bmp);
	gfx::Bitmap cpy2 = gfx::BitmapCopy(bmp);
	gfx::Bitmap display = gfx::BitmapCreate(1024, 1024);

	gfx::BitmapBlit(
		cpy1,
		{ 0, 0, gfx::BitmapGetWidth(cpy1), gfx::BitmapGetHeight(cpy1) },
		display,
		{ 0, 0 }
	);

	gfx::BitmapBlit(
		cpy2,
		{ 0, 0, gfx::BitmapGetWidth(cpy2), gfx::BitmapGetHeight(cpy2) },
		display,
		{ gfx::BitmapGetWidth(cpy1), 0 }
	);

	gfx::PutPixel(display, 0, 0, gfx::MakeColor(255, 255, 255, 255));

	while (!close)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
				close = true;
			else if (event.type == SDL_EVENT_WINDOW_RESIZED)
				gfx::RaiseWindowResizeEvent();
		}

		gfx::BitmapBlit(
			display,
			{ 0, 0, gfx::BitmapGetWidth(display), gfx::BitmapGetHeight(display) },
			gfx::GetScreenBuffer(),
			{ 0, 0 }
		);

		//gfx::BitmapBlit(
		//	bmp,
		//	{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
		//	gfx::GetScreenBuffer(),
		//	{ 40, 40 }
		//);

		//gfx::BitmapBlit(
		//	bmp,
		//	{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
		//	gfx::GetScreenBuffer(),
		//	{ 500, 500 }
		//);

		//gfx::BitmapBlit(
		//	bmp,
		//	{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
		//	gfx::GetScreenBuffer(),
		//	{ 40, 500 }
		//);

		//gfx::BitmapBlit(
		//	bmp,
		//	{ 0, 0, gfx::BitmapGetWidth(bmp), gfx::BitmapGetHeight(bmp) },
		//	gfx::GetScreenBuffer(),
		//	{ 500, 40 }
		//);

		gfx::Flush();
	}

	gfx::BitmapDestroy(cpy1);
	gfx::BitmapDestroy(cpy2);
	gfx::BitmapDestroy(display);
	gfx::Close();

	return 0;
}