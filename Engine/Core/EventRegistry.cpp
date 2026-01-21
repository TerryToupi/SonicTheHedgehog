#include "Core/EventRegistry.h"
#include "SDL3/SDL.h"

namespace core
{ 
	MouseEvent		EventRegistry::s_mouseEvents;
	KeyEvent		EventRegistry::s_keyEvents;
	CloseEvent		EventRegistry::s_closeEvents;
	ResizeEvent		EventRegistry::s_resizeEvents;
	PauseEvent		EventRegistry::s_pauseEvents;
	ControllerEvent	EventRegistry::s_controllerEvents;

	void EventRegistry::Update()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
				s_closeEvents.Emit();

			else if (event.type == SDL_EVENT_WINDOW_RESIZED)
				s_resizeEvents.Emit(event.window.data1, event.window.data2);
		}
	}

	void EventRegistry::Unsubscribe(EventType type, int id)
	{
		switch (type)
		{
		case EventType::MOUSE_EVENT:
			s_mouseEvents.Unsubscribe(id);
			break;
		case EventType::KEY_EVENT:
			s_keyEvents.Unsubscribe(id);
			break;
		case EventType::CLOSE_EVENT:
			s_closeEvents.Unsubscribe(id);
			break;
		case EventType::RESIZE_EVENT:
			s_resizeEvents.Unsubscribe(id);
			break;
		case EventType::PAUSE_EVENT:
			s_pauseEvents.Unsubscribe(id);
			break;
		case EventType::CONTROLLER_EVENT:
			s_controllerEvents.Unsubscribe(id);
			break;
		default:
			break;
		}
	}

	EventHandle::~EventHandle()
	{
		if (type !=  EventType::UNKNOWN_EVENT)
			EventRegistry::Unsubscribe(type, id);
	}
}
