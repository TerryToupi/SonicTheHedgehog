#include "Rendering/Renderer.h"
#include "Scene/SpriteManager.h"
#include "Scene/Sprite.h"
#include "Core/EventRegistry.h"
#include "Core/Input.h"
#include "IO/IOMapping.h"

#include <string>

#include <iostream>

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

		auto h1 = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT, 
			[&close]() { 
				close = true;
				std::cout << "Application closing!";
			});

		auto h2 = core::EventRegistry::Subscribe(EventType::RESIZE_EVENT, 
			[](int w, int h) { 
				gfx::RaiseWindowResizeEvent(); 
				std::cout << "Raised window resize event with x: " << w << " and height y: " << h << std::endl;
			});

		auto h3 = core::EventRegistry::Subscribe(EventType::MOUSE_MORION_EVENT,
			[](int x, int y) {
				std::cout << "[Mouse motion] x: " << x << " y: " << y << std::endl;
			});

		auto h4 = core::EventRegistry::Subscribe(EventType::MOUSE_BUTTON_EVENT,
			[](io::Button button) {
				std::cout << "[Mouse button] button: " << std::endl;
			});

		auto h5 = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
			[](io::Key key) {
				if (key == io::Key::A)
					std::cout << "[Key event] key: A" << std::endl;
			});

		while (!close)
		{ 
			core::Input::Update();

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