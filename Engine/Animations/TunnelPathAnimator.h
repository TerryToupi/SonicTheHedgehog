#pragma once

#include "Utils/Common.h"
#include "Animations/Animator.h"
#include "Animations/TunnelPath.h"

namespace anim
{
	class TunnelPathAnimator : public Animator
	{
	public:
		virtual void Progress(TimeStamp currTime) override;

		void Start(const TunnelPath* path, TimeStamp t);

		// Get current interpolated position along the path
		Point GetCurrentPosition() const;

		// Get movement direction (for sprite facing)
		// Returns {-1, 0, 1} for x and y components
		Point GetDirection() const;

		// Get current speed
		float GetCurrentSpeed() const { return m_CurrentSpeed; }

		// Get progress through path (0.0 to 1.0)
		float GetProgress() const;

		TunnelPathAnimator();

	private:
		// Interpolate position based on current distance traveled
		Point InterpolatePosition() const;

		const TunnelPath* m_Path = nullptr;
		float m_Distance = 0.0f;         // Distance traveled along path
		float m_TotalLength = 0.0f;      // Total path length
		float m_CurrentSpeed = 0.0f;     // Current movement speed
		unsigned m_CurrentSegment = 0;   // Current waypoint segment index
		float m_SegmentStart = 0.0f;     // Distance at start of current segment
	};
}
