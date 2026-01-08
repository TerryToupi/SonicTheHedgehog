#pragma once

#include "Utils/Common.h"
#include "Core/Game.h"

namespace core
{
	class Context 
	{
	public:
		virtual void Initialize(void) = 0;
		virtual void Load(void) = 0;
		virtual void Run(void);
		virtual void RunIteration(void);

		const Game& GetGame(void) const;
		Game&		GetGame(void);

		virtual void Clear(void) = 0;

		void Start(void);

		Context(void) = default;
		Context(const Context&) = delete;
		Context(Context&&) = delete;

	protected:
		Game m_Game;
	};
}