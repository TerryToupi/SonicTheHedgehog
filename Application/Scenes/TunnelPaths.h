#pragma once

#include "Animations/TunnelPath.h"

#include <vector>

namespace TunnelPaths
{
	// Loop shape - relative offsets from entry point (left to right)
	inline const std::vector<Point> LOOP_SHAPE = {
		{0, 0},
		{37, 11},
		{82, 4},
		{112, -7},
		{142, -31},
		{153, -60},
		{153, -88},
		{140, -112},
		{120, -131},
		{93, -138},
		{67, -135},
		{44, -122},
		{29, -102},
		{22, -81},
		{24, -60},
		{33, -39},
		{46, -20},
		{65, -4},
		{84, 6},
		{115, 10},
		{178, 0},
		{188, 0},
		{198, 0}

	};

	// Loop shape inverse - mirrored for right to left
	inline const std::vector<Point> LOOP_SHAPE_INVERSE = {
		{0, 0},
		{-37, 11},
		{-82, 4},
		{-112, -7},
		{-142, -31},
		{-153, -60},
		{-153, -88},
		{-140, -112},
		{-120, -131},
		{-93, -138},
		{-67, -135},
		{-44, -122},
		{-29, -102},
		{-22, -81},
		{-24, -60},
		{-33, -39},
		{-46, -20},
		{-65, -4},
		{-84, 6},
		{-115, 10},
		{-178, 0},
		{-188, 0},
		{-198, 0}
		
	};

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

		// S-curve tunnels (direction 1 = moving right)
		tunnels.push_back(anim::CreateTunnelAt("tunnel_1", {6170, 878}, S_CURVE_SHAPE, 1));
		tunnels.push_back(anim::CreateTunnelAt("tunnel_1", {6682, 1134}, S_CURVE_SHAPE, 1));
		tunnels.push_back(anim::CreateTunnelAt("tunnel_1", {6682, 622}, S_CURVE_SHAPE, 1));

		// Loop (positions are Sonic's center: box + {15, 20})
		// Forward loop requires moving right (1), inverse requires moving left (-1)
		tunnels.push_back(anim::CreateTunnelAt("loop_1", {5410, 930}, LOOP_SHAPE, 1));
		tunnels.push_back(anim::CreateTunnelAt("loop_1_inv", {5585, 930}, LOOP_SHAPE_INVERSE, -1));

		return tunnels;
	}
}
