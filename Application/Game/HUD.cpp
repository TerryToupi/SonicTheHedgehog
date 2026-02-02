#include "Game/HUD.h"
#include "Game/GameStats.h"
#include "Utilities/DrawHelpers.h"
#include "Rendering/Color.h"

#include <cstdio>

void HUD::Render(gfx::Bitmap screen)
{
    RenderScore(screen);
    RenderTime(screen);
    RenderRings(screen);
}

void HUD::RenderScore(gfx::Bitmap screen)
{
    // Yellow color matching original Sonic HUD
    gfx::Color yellow = gfx::MakeColor(255, 255, 0, 255);

    // Label
    draw::Text(screen, HUD_X, HUD_Y, "SCORE", yellow, FONT_SCALE);

    // Value - format score with leading spaces for alignment
    char scoreStr[16];
    snprintf(scoreStr, sizeof(scoreStr), "%d", GameStats::Get().GetScore());
    draw::Text(screen, VALUE_X, HUD_Y, scoreStr, yellow, FONT_SCALE);
}

void HUD::RenderTime(gfx::Bitmap screen)
{
    gfx::Color yellow = gfx::MakeColor(255, 255, 0, 255);

    // Label
    int y = HUD_Y + LINE_HEIGHT;
    draw::Text(screen, HUD_X, y, "TIME", yellow, FONT_SCALE);

    // Value - format as M:SS
    int minutes = GameStats::Get().GetTimeMinutes();
    int seconds = GameStats::Get().GetTimeSeconds();
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%d:%02d", minutes, seconds);
    draw::Text(screen, VALUE_X, y, timeStr, yellow, FONT_SCALE);
}

void HUD::RenderRings(gfx::Bitmap screen)
{
    gfx::Color yellow = gfx::MakeColor(255, 255, 0, 255);

    // Label
    int y = HUD_Y + LINE_HEIGHT * 2;
    draw::Text(screen, HUD_X, y, "RINGS", yellow, FONT_SCALE);

    // Value
    char ringsStr[16];
    snprintf(ringsStr, sizeof(ringsStr), "%d", GameStats::Get().GetRings());
    draw::Text(screen, VALUE_X, y, ringsStr, yellow, FONT_SCALE);
}
