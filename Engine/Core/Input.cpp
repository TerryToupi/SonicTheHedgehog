#include "Core/Input.h"
#include "IO/IOMapping.h"

#include "Core/EventRegistry.h"
#include "SDL3/SDL.h"

namespace core
{
	void Input::Update()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
				EventRegistry::EmitCloseEvents();

			else if (event.type == SDL_EVENT_WINDOW_RESIZED)
				EventRegistry::EmitResizeEvents(event.window.data1, event.window.data2);

			else if (event.type == SDL_EVENT_WINDOW_MINIMIZED)
				EventRegistry::EmitPauseEvents();

			else if (event.type == SDL_EVENT_KEY_DOWN)
				EventRegistry::EmitKeyEvents(io::IOMapper::ConvertKey(static_cast<int>(event.key.scancode)));

			else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
				EventRegistry::EmitMouseButtonEvents(io::IOMapper::ConvertButton(static_cast<int>(event.button.button)));

			else if (event.type == SDL_EVENT_MOUSE_MOTION)
				EventRegistry::EmitMouseMotionEvents(event.motion.x, event.motion.y);
		}
	}
}
