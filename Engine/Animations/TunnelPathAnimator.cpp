#include "Animations/TunnelPathAnimator.h"

#include <cmath>

namespace anim
{
	TunnelPathAnimator::TunnelPathAnimator()
		: Animator()
	{
	}

	void TunnelPathAnimator::Start(const TunnelPath* path, TimeStamp t)
	{
		m_Path = path;
		m_LastTime = t;
		m_State = ANIMATOR_RUNNING;
		m_Distance = 0.0f;
		m_CurrentSegment = 0;
		m_SegmentStart = 0.0f;
		m_TotalLength = path->GetTotalLength();
		m_CurrentSpeed = path->baseSpeed;
		NotifyStarted();
	}

	void TunnelPathAnimator::Progress(TimeStamp currTime)
	{
		if (m_State != ANIMATOR_RUNNING || !m_Path)
			return;

		// Time-based progression (roughly 60fps assumption)
		// We advance by m_CurrentSpeed pixels per ~16ms
		TimeStamp elapsed = currTime - m_LastTime;
		if (elapsed < 16) // Cap at ~60fps update rate
			return;

		m_LastTime = currTime;

		// Calculate progress ratio (0 to 1)
		float progress = m_TotalLength > 0 ? m_Distance / m_TotalLength : 0.0f;

		// Ramp speed based on progress (linear interpolation)
		m_CurrentSpeed = m_Path->baseSpeed +
			progress * (m_Path->maxSpeed - m_Path->baseSpeed);

		// Advance distance
		m_Distance += m_CurrentSpeed;

		// Update current segment if we've passed waypoints
		while (m_CurrentSegment + 1 < m_Path->waypoints.size())
		{
			const auto& p1 = m_Path->waypoints[m_CurrentSegment];
			const auto& p2 = m_Path->waypoints[m_CurrentSegment + 1];
			int dx = p2.x - p1.x;
			int dy = p2.y - p1.y;
			float segmentLen = std::sqrt(static_cast<float>(dx * dx + dy * dy));

			if (m_Distance >= m_SegmentStart + segmentLen)
			{
				m_SegmentStart += segmentLen;
				++m_CurrentSegment;
			}
			else
			{
				break;
			}
		}

		// Notify action callback with current state
		NotifyAction(nullptr);

		// Check if we've reached the end
		if (m_Distance >= m_TotalLength)
		{
			m_State = ANIMATOR_FINISHED;
			NotifyStopped();
		}
	}

	Point TunnelPathAnimator::GetCurrentPosition() const
	{
		return InterpolatePosition();
	}

	Point TunnelPathAnimator::InterpolatePosition() const
	{
		if (!m_Path || m_Path->waypoints.empty())
			return {0, 0};

		if (m_Path->waypoints.size() == 1)
			return m_Path->waypoints[0];

		// Clamp to valid segment
		size_t seg = m_CurrentSegment;
		if (seg + 1 >= m_Path->waypoints.size())
			return m_Path->waypoints.back();

		const auto& p1 = m_Path->waypoints[seg];
		const auto& p2 = m_Path->waypoints[seg + 1];

		// Calculate segment length
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;
		float segmentLen = std::sqrt(static_cast<float>(dx * dx + dy * dy));

		if (segmentLen < 0.001f)
			return p1;

		// Calculate position within segment
		float distInSegment = m_Distance - m_SegmentStart;
		float t = distInSegment / segmentLen;
		t = std::min(1.0f, std::max(0.0f, t)); // Clamp to [0, 1]

		return {
			static_cast<int>(p1.x + t * dx),
			static_cast<int>(p1.y + t * dy)
		};
	}

	Point TunnelPathAnimator::GetDirection() const
	{
		if (!m_Path || m_Path->waypoints.size() < 2)
			return {1, 0}; // Default: facing right

		size_t seg = m_CurrentSegment;
		if (seg + 1 >= m_Path->waypoints.size())
			seg = m_Path->waypoints.size() - 2;

		const auto& p1 = m_Path->waypoints[seg];
		const auto& p2 = m_Path->waypoints[seg + 1];

		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;

		// Return normalized direction (-1, 0, or 1)
		return {
			(dx > 0) ? 1 : (dx < 0) ? -1 : 0,
			(dy > 0) ? 1 : (dy < 0) ? -1 : 0
		};
	}

	float TunnelPathAnimator::GetProgress() const
	{
		if (m_TotalLength <= 0)
			return 0.0f;
		return std::min(1.0f, m_Distance / m_TotalLength);
	}
}
