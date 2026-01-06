#include "Core/LatelyDestroyable.h"
#include "Utils/Assert.h"

namespace core
{
	void DestructionManager::Register(LatelyDestroyable* d)
	{
		ASSERT(!d->IsAlive(), "Faild. Commited object for destruction is not dying!");
		m_Dead.push_back(d);
	}

	void DestructionManager::Commit(void)
	{
		for (auto* d : m_Dead)
			d->Delete();
		m_Dead.clear();
	}

	auto DestructionManager::Get(void) -> DestructionManager&
	{
		return s_DestructionManager;
	}

	bool LatelyDestroyable::IsAlive(void) const
	{
		return m_Alive;
	}

	void LatelyDestroyable::Destroy(void)
	{
		if (m_Alive)
		{
			m_Alive = false;
			DestructionManager::Get().Register(this);
		}
	}

	void LatelyDestroyable::Delete(void)
	{
		ASSERT(!m_Dying, "Failed. Object is already dying!");
		m_Dying = true;
		delete this;
	}

	LatelyDestroyable::~LatelyDestroyable()
	{
		ASSERT(m_Dying, "FAILED. Object tried to be destroyed with out the Destruction manager!");
	}
}

