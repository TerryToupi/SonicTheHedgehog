#pragma once

#include "Utils/Common.h"

#include <vector>
#include <string>

namespace anim
{
	struct TunnelPath
	{
		std::string id;
		std::vector<Point> waypoints;  // Absolute positions along the path
		Rect triggerBox;               // Entry detection zone
		float baseSpeed = 8.0f;        // Starting speed (pixels per frame)
		float maxSpeed = 16.0f;        // Maximum speed at end of path

		// Calculate total path length
		float GetTotalLength() const
		{
			float length = 0.0f;
			for (size_t i = 1; i < waypoints.size(); ++i)
			{
				int dx = waypoints[i].x - waypoints[i - 1].x;
				int dy = waypoints[i].y - waypoints[i - 1].y;
				length += std::sqrt(static_cast<float>(dx * dx + dy * dy));
			}
			return length;
		}

		// Check if a point is inside the trigger box
		bool IsInTrigger(int x, int y) const
		{
			return x >= triggerBox.x && x < triggerBox.x + triggerBox.w &&
			       y >= triggerBox.y && y < triggerBox.y + triggerBox.h;
		}
	};

	// Helper to create a tunnel path from relative offsets
	inline TunnelPath CreateTunnelAt(
		const std::string& id,
		Point startPos,
		const std::vector<Point>& relativeShape,
		float baseSpeed = 8.0f,
		float maxSpeed = 16.0f,
		int triggerSize = 24)
	{
		TunnelPath path;
		path.id = id;
		path.baseSpeed = baseSpeed;
		path.maxSpeed = maxSpeed;

		// Trigger box centered on start position
		path.triggerBox = {
			startPos.x - triggerSize / 2,
			startPos.y - triggerSize / 2,
			triggerSize,
			triggerSize
		};

		// Convert relative offsets to absolute positions
		for (const auto& offset : relativeShape)
		{
			path.waypoints.push_back({
				startPos.x + offset.x,
				startPos.y + offset.y
			});
		}

		return path;
	}
}
