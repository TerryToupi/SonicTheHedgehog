#pragma once

#include <string>

namespace anim
{
	class Animation
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