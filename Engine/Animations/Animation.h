#pragma once

#include "Core/LatelyDestroyable.h"

#include <string>

namespace anim
{
	using namespace core;

	class Animation : public LatelyDestroyable
	{
	public:
		const std::string& GetID();
		void SetID(const std::string& id);

		virtual Animation* Clone(void) const = 0;

		Animation(const std::string& id);
		virtual ~Animation() = default;

	protected:
		std::string m_ID;
	};
}