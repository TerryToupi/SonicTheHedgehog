#pragma once

#include "Utils/Event.h"

namespace core
{
	class MouseEvent final : public Event<int, int>
	{
	public:
		EventType Type() override { return EventType::MOUSE_EVENT; }
	};

	class KeyEvent final : public Event<int>
	{
	public:
		EventType Type() override { return EventType::KEY_EVENT; }
	};

	class CloseEvent final : public Event<>
	{
	public:
		EventType Type() override { return EventType::CLOSE_EVENT; }
	};

	class ResizeEvent final : public Event<int, int>
	{
	public:
		EventType Type() override { return EventType::RESIZE_EVENT; }
	};

	class PauseEvent final : public Event<>
	{
	public:
		EventType Type() override { return EventType::PAUSE_EVENT; }
	};

	class ControllerEvent final : public Event<>
	{
	public:
		EventType Type() override { return EventType::CONTROLLER_EVENT; }
	};

	struct EventHandle
	{
		EventType type = EventType::UNKNOWN_EVENT;
		int id = 0;

		~EventHandle();
	};

	class EventRegistry final
	{
	public:
		template<typename ...Args>
		static EventHandle Subscribe(EventType type, std::function<void(Args...)> listener)
		{
			if constexpr (sizeof...(Args) == 2 &&
				std::is_same_v<std::tuple<Args...>, std::tuple<int, int>>)
			{
				switch (type)
				{
				case EventType::MOUSE_EVENT:
					return { EventType::MOUSE_EVENT, s_mouseEvents.Subscribe(std::move(listener)) };
				case EventType::RESIZE_EVENT:
					return { EventType::RESIZE_EVENT, s_resizeEvents.Subscribe(std::move(listener)) };
				default:
					return { EventType::UNKNOWN_EVENT, -1 };
				}
			}

			else if constexpr (sizeof...(Args) == 1 &&
				std::is_same_v<std::tuple<Args...>, std::tuple<int>>)
			{
				switch (type)
				{
				case EventType::KEY_EVENT:
					return { EventType::KEY_EVENT, s_keyEvents.Subscribe(std::move(listener)) };
				default:
					return { EventType::UNKNOWN_EVENT, -1 };
				}
			}

			else if constexpr (sizeof...(Args) == 0)
			{
				switch (type)
				{
				case EventType::CLOSE_EVENT:
					return { EventType::CLOSE_EVENT, s_closeEvents.Subscribe(std::move(listener)) };
				case EventType::PAUSE_EVENT:
					return { EventType::PAUSE_EVENT, s_pauseEvents.Subscribe(std::move(listener)) };
				case EventType::CONTROLLER_EVENT:
					return { EventType::CONTROLLER_EVENT, s_controllerEvents.Subscribe(std::move(listener)) };
				default:
					return { EventType::UNKNOWN_EVENT, -1 };
				}
			}

			else
			{
				static_assert(sizeof...(Args) >= 0, "FAILED. Unsuported listener format!");
				return -1;
			}
		}

		template <typename F>
		static EventHandle Subscribe(EventType type, F&& listener)
		{
			using Fn = std::decay_t<F>;

			if constexpr (std::is_invocable_r_v<void, Fn>) {
				return Subscribe<>(type, std::function<void()>(std::forward<F>(listener)));
			}
			else if constexpr (std::is_invocable_r_v<void, Fn, int>) {
				return Subscribe<int>(type, std::function<void(int)>(std::forward<F>(listener)));
			}
			else if constexpr (std::is_invocable_r_v<void, Fn, int, int>) {
				return Subscribe<int, int>(type, std::function<void(int, int)>(std::forward<F>(listener)));
			}
			else {
				static_assert(sizeof(F) == 0, "Unsupported listener signature");
			}
		}

		static void Update();

	private:
		static void Unsubscribe(EventType type, int id);

	private:
		EventRegistry() = default;
		EventRegistry(const EventRegistry&) = delete;
		EventRegistry(EventRegistry&&) = delete;

	private:
		static MouseEvent		s_mouseEvents;
		static KeyEvent			s_keyEvents;
		static CloseEvent		s_closeEvents;
		static ResizeEvent		s_resizeEvents;
		static PauseEvent		s_pauseEvents;
		static ControllerEvent	s_controllerEvents;

		friend class Game;
		friend class Context;
		friend class EventHandle;
	};
}
