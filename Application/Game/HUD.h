#pragma once

#include "Rendering/Bitmap.h"

class HUD
{
public:
    void Render(gfx::Bitmap screen);

private:
    void RenderScore(gfx::Bitmap screen);
    void RenderTime(gfx::Bitmap screen);
    void RenderRings(gfx::Bitmap screen);

    // Layout constants - compact style with small font
    static constexpr int HUD_X = 8;
    static constexpr int HUD_Y = 8;
    static constexpr int LINE_HEIGHT = 10;
    static constexpr int VALUE_X = 48;  // X position for values
    static constexpr int FONT_SCALE = 1;
};
