#pragma once

#include <list>

namespace core
{
	class LatelyDestroyable;

	class DestructionManager
	{
	public:
		static auto Get(void) -> DestructionManager&;

		void Register(LatelyDestroyable* d);
		void Commit(void);

	private:
		static DestructionManager s_DestructionManager;

		std::list<LatelyDestroyable*> m_Dead;
	};

	class LatelyDestroyable
	{
	public:
		LatelyDestroyable(void) = default;

		bool IsAlive(void) const;
		void Destroy(void);

	protected:
		void Delete(void);

	protected:
		virtual ~LatelyDestroyable();

		bool m_Alive = true;
		bool m_Dying = false;

		friend class DestructionManager;
	};
}