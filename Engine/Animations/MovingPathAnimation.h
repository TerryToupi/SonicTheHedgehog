#pragma once

#include "Animations/Animation.h"

#include <vector>

namespace anim
{
	class MovingPathAnimation : public Animation 
	{
	public:
		struct PathEntry
		{
			int dx = 0;
			int dy = 0;
			unsigned frame = 0;
			unsigned delay = 0;
		};

		using Path = std::vector<PathEntry>;
		using Me = MovingPathAnimation;

	public:
		const Path& GetPath(void) const;
		Me& SetPath(const Path& p);

		Animation* Clone(void) const override;

		MovingPathAnimation(
			const std::string& id,
			const Path& path
		);

	private:
		Path m_Path;
	};
}