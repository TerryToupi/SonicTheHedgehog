#include "Core/Input.h"
#include "IO/IOMapping.h"

#include "Core/EventRegistry.h"
#include "SDL3/SDL.h"

namespace core
{
	void Input::UpdateInputEvents()
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

	void Input::FlushEvents()
	{
		// Drain all pending events without processing them
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Only process quit events to allow closing during transitions
			if (event.type == SDL_EVENT_QUIT)
				EventRegistry::EmitCloseEvents();
		}
	}

	bool Input::IsKeyPressed(io::Key key)
	{
		const bool* keystate = SDL_GetKeyboardState(nullptr);
		int scancode = io::IOMapper::GetScancode(key);
		return scancode != SDL_SCANCODE_UNKNOWN && keystate[scancode];
	}

	void Input::GetMousePosition(int* x, int* y)
	{
		float fx, fy;
		SDL_GetMouseState(&fx, &fy);
		if (x) *x = static_cast<int>(fx);
		if (y) *y = static_cast<int>(fy);
	}
}
