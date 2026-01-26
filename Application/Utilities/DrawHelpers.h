#pragma once

#include "Rendering/Bitmap.h"
#include "Rendering/Color.h"

#include <cstdint>

namespace draw
{
    // Font data (5x7 pixel font for A-Z, 0-9)
    extern const uint8_t FONT_5X7[][7];

    // Get font index for a character (-1 if not found)
    int GetFontIndex(char c);

    // Direct pixel manipulation (caller must lock/unlock bitmap)
    void PutPixelDirect(uint8_t* base, int pitch, int x, int y,
                        gfx::Color color, int screenW, int screenH);

    // Draw a filled rectangle
    void FilledRect(gfx::Bitmap screen, int x, int y, int w, int h, gfx::Color color);

    // Draw a rectangle border
    void RectBorder(gfx::Bitmap screen, int x, int y, int w, int h,
                    gfx::Color color, int thickness = 2);

    // Draw a 3D stone-style button
    void StoneButton(gfx::Bitmap screen, int x, int y, int w, int h, bool selected);

    // Draw text using the built-in pixel font
    void Text(gfx::Bitmap screen, int x, int y, const char* text,
              gfx::Color color, int scale = 1);

    // Draw a selection arrow (pointing right)
    void Arrow(gfx::Bitmap screen, int x, int y, gfx::Color color);
}
