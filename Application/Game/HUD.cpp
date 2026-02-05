#include "Game/HUD.h"
#include "Game/GameStats.h"
#include "Utilities/DrawHelpers.h"
#include "Rendering/Color.h"
#include "Animations/AnimationFilmHolder.h"

#include <cstdio>

void HUD::Render(gfx::Bitmap screen, int viewportHeight)
{
    RenderScore(screen);
    RenderTime(screen);
    RenderRings(screen);
    RenderLives(screen, viewportHeight);
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

void HUD::RenderLives(gfx::Bitmap screen, int viewportHeight)
{
    // Get the sonic face film
    const anim::AnimationFilm* faceFilm =
        anim::AnimationFilmHolder::Get().GetFilm("sonic.face.overlay");

    if (!faceFilm)
        return;

    // Position at bottom-left
    int x = LIVES_MARGIN;
    int y = viewportHeight - LIVES_ICON_SIZE - LIVES_MARGIN;

    // Draw the face icon
    faceFilm->DisplayFrame(screen, {x, y}, 0);

    // Draw "x" and lives count next to the icon
    gfx::Color yellow = gfx::MakeColor(255, 255, 0, 255);
    int textX = x + LIVES_ICON_SIZE + 2;
    int textY = y + 4;  // Center text vertically with icon

    char livesStr[16];
    snprintf(livesStr, sizeof(livesStr), "X%d", GameStats::Get().GetLives());
    draw::Text(screen, textX, textY, livesStr, yellow, FONT_SCALE);
}
