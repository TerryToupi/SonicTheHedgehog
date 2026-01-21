#include "Core/EventRegistry.h"
#include "SDL3/SDL.h"

namespace core
{ 
	MouseMotionEvent EventRegistry::s_mouseMotionEvents;
	MouseButtonEvent EventRegistry::s_mouseButtonEvents;
	KeyEvent		 EventRegistry::s_keyEvents;
	CloseEvent		 EventRegistry::s_closeEvents;
	ResizeEvent		 EventRegistry::s_resizeEvents;
	PauseEvent		 EventRegistry::s_pauseEvents;
	ControllerEvent	 EventRegistry::s_controllerEvents;

	void EventRegistry::Unsubscribe(EventType type, int id)
	{
		switch (type)
		{
		case EventType::MOUSE_MORION_EVENT:
			s_mouseMotionEvents.Unsubscribe(id);
			break;
		case EventType::MOUSE_BUTTON_EVENT:
			s_mouseButtonEvents.Unsubscribe(id);
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

	void EventRegistry::EmitMouseMotionEvents(int x, int y)
	{
		s_mouseMotionEvents.Emit(x, y);
	}

	void EventRegistry::EmitResizeEvents(int x, int y)
	{
		s_resizeEvents.Emit(x, y);
	}

	void EventRegistry::EmitKeyEvents(io::Key key)
	{
		s_keyEvents.Emit(key);
	}

	void EventRegistry::EmitCloseEvents(void)
	{
		s_closeEvents.Emit();
	}

	void EventRegistry::EmitPauseEvents(void)
	{
		s_pauseEvents.Emit();
	}

	void EventRegistry::EmitControllerEvents(void)
	{
		s_controllerEvents.Emit();
	}

	void EventRegistry::EmitMouseButtonEvents(io::Button button)
	{
		s_mouseButtonEvents.Emit(button);
	}

	EventHandle::~EventHandle()
	{
		if (type !=  EventType::UNKNOWN_EVENT)
			EventRegistry::Unsubscribe(type, id);
	}
}
