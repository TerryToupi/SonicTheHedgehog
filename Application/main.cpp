#include <SDL3/SDL.h>

#include "Rendering/Renderer.h"
#include "Scene/SpriteManager.h"
#include "Scene/Sprite.h"
#include "Core/EventRegistry.h"

#include <string>

int main(void)
{ 
	bool close = false;
	{ 

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

		auto h1 = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT, [&close]() { close = true; });
		auto h2 = core::EventRegistry::Subscribe(EventType::RESIZE_EVENT, [](int w, int h) { gfx::RaiseWindowResizeEvent(); });

		while (!close)
		{ 
			core::EventRegistry::Update(); 

			gfx::BitmapBlit(
				display,
				{ 0, 0, gfx::BitmapGetWidth(display), gfx::BitmapGetHeight(display) },
				gfx::GetScreenBuffer(),
				{ 0, 0 }
			);

			gfx::Flush();
		}

		gfx::BitmapDestroy(cpy1);
		gfx::BitmapDestroy(cpy2);
		gfx::BitmapDestroy(display);
		gfx::Close();
	}

	return 0;
}