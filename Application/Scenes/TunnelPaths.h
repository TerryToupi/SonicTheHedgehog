#pragma once

#include "Animations/TunnelPath.h"

#include <vector>

namespace TunnelPaths
{
	// Defining the S-curve shape ONCE as relative offsets from entry point.
	// Each point is {dx, dy} relative to the tunnel entry position.
	inline const std::vector<Point> S_CURVE_SHAPE = {
		// Entry point (always {0, 0})
		{0, 0},
		{38, 2},
		{70, 18},
		{91, 49},
		{100, 74},
		{91, 98},
		{77, 118},
		{60, 141},

		// Mid-way
		{55, 161},
		{55, 198},
		{71, 228},
		{94, 248},
		{124, 257},
		{199, 257}
	};

	// Create all tunnel paths in the level
	inline std::vector<anim::TunnelPath> CreateAllTunnels()
	{
		std::vector<anim::TunnelPath> tunnels;

		tunnels.push_back(anim::CreateTunnelAt("tunnel_1", {6170, 878}, S_CURVE_SHAPE));
		tunnels.push_back(anim::CreateTunnelAt("tunnel_1", {6682, 1134}, S_CURVE_SHAPE));
		tunnels.push_back(anim::CreateTunnelAt("tunnel_1", {6682, 622}, S_CURVE_SHAPE));


		return tunnels;
	}
}
