#pragma once

#include "Animations/Animation.h"

#include <vector>

namespace anim
{
	class ScrollAnimation : public Animation
	{
	public:
		struct ScrollEntry
		{
			int dx = 0;
			int dy = 0;
			unsigned delay = 0;
		};

		using Scroll = std::vector<ScrollEntry>;
		using Me = ScrollAnimation;

	public:
		const Scroll& GetScroll(void) const;
		Me& SetScroll(const Scroll& p);

		Animation* Clone(void) const override;

		ScrollAnimation(
			const std::string& id,
			const Scroll& scroll
		);

	private:
		Scroll m_Scroll;
	};
}