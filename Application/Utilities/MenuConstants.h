#pragma once

#include "Rendering/Color.h"

namespace menu
{
    // Button colors (Stone-like Sonic aesthetic)
    constexpr gfx::Color COLOR_BUTTON_FACE = 0xFF808080;       // Grey stone face
    constexpr gfx::Color COLOR_BUTTON_LIGHT = 0xFFC0C0C0;      // Light grey highlight (top/left)
    constexpr gfx::Color COLOR_BUTTON_DARK = 0xFF404040;       // Dark grey shadow (bottom/right)
    constexpr gfx::Color COLOR_BUTTON_SELECTED = 0xFFA0A0A0;   // Lighter grey when selected
    constexpr gfx::Color COLOR_BUTTON_SEL_LIGHT = 0xFFE0E0E0;  // Brighter highlight when selected
    constexpr gfx::Color COLOR_TEXT = 0xFF000000;              // Black text
    constexpr gfx::Color COLOR_TEXT_SHADOW = 0xFF303030;       // Dark shadow for text
    constexpr gfx::Color COLOR_ARROW = 0xFF00D0FF;             // Cyan arrow (Sonic blue)

    // Button configuration
    struct Button
    {
        int x, y, width, height;
    };

    // Check if point is inside a button
    inline bool IsPointInButton(int px, int py, const Button& btn)
    {
        return px >= btn.x && px < btn.x + btn.width &&
               py >= btn.y && py < btn.y + btn.height;
    }
}
