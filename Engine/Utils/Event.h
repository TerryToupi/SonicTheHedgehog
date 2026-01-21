#pragma once

#include <vector>
#include <functional> 

#include "Utils/EventTypes.h"

template<typename... Args>
class Event
{
public:
	using Listener = std::function<void(Args...)>;

	virtual EventType Type() = 0;

	int Subscribe(Listener listener)
	{
		const int id = Acquire_id();
		m_listeners.push_back({ id, std::move(listener) });
		return id;
	}

	void Unsubscribe(int id)
	{
		for (int i = 0; i < m_listeners.size(); ++i)
		{
			if (m_listeners[i].id == id)
			{
				m_free_ids.push_back(id);
				m_listeners.erase(m_listeners.begin() + i);
				return;
			}
		}
	}

	void Emit(Args... args) const
	{
		if (!m_listeners.empty())
			for (int i = 0; i < m_listeners.size(); ++i)
				m_listeners[i].listener(args...);
	}

	Event() = default;
	Event(const Event&) = default;
	Event(Event&&) = default;

private:
	int Acquire_id()
	{
		if (!m_free_ids.empty())
		{
			int id = m_free_ids.back();
			m_free_ids.pop_back();
			return id;
		}

		int id = m_next_id;
		++m_next_id;
		return id;
	}

private:
	struct Entry
	{
		int		 id;
		Listener listener;
	};

	int					m_next_id = 0;
	std::vector<int>	m_free_ids;
	std::vector<Entry>	m_listeners;
};