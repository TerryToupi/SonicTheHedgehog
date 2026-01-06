#pragma once

#include "Animations/MovingAnimation.h"

#include <vector>

namespace anim
{
	class FrameListAnimation : public MovingAnimation
	{
	public:
		using Frames = std::vector<unsigned>;
		using Me = FrameListAnimation;

	public:
		const Frames& GetFrames(void) const;
		Me& SetFrames(const Frames& f);

		Animation* Clone(void) const override;

		FrameListAnimation(
			const std::string& id,
			const Frames& frames,
			unsigned r, int dx, int dy, unsigned d
		);

	protected:
		Frames m_Frames;
	};
}
