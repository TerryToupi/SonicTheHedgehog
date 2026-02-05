#pragma once

#include "Rendering/Bitmap.h"

class HUD
{
public:
    void Render(gfx::Bitmap screen, int viewportHeight);

private:
    void RenderScore(gfx::Bitmap screen);
    void RenderTime(gfx::Bitmap screen);
    void RenderRings(gfx::Bitmap screen);
    void RenderLives(gfx::Bitmap screen, int viewportHeight);

    // Layout constants - compact style with small font
    static constexpr int HUD_X = 8;
    static constexpr int HUD_Y = 8;
    static constexpr int LINE_HEIGHT = 10;
    static constexpr int VALUE_X = 48;  // X position for values
    static constexpr int FONT_SCALE = 1;

    // Lives display constants (bottom-left)
    static constexpr int LIVES_MARGIN = 8;
    static constexpr int LIVES_ICON_SIZE = 16;
};
