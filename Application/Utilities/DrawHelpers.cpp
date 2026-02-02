#include "Utilities/DrawHelpers.h"
#include "Utilities/MenuConstants.h"

namespace draw
{
    // Simple 5x7 pixel font for text (A-Z, 0-9, colon)
    const uint8_t FONT_5X7[][7] = {
        { 0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 },  // A
        { 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110 },  // B
        { 0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110 },  // C
        { 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110 },  // D
        { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 },  // E
        { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 },  // F
        { 0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110 },  // G
        { 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 },  // H
        { 0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 },  // I
        { 0b00111, 0b00010, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100 },  // J
        { 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 },  // K
        { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 },  // L
        { 0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001 },  // M
        { 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001 },  // N
        { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 },  // O
        { 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 },  // P
        { 0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101 },  // Q
        { 0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001 },  // R
        { 0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110 },  // S
        { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 },  // T
        { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 },  // U
        { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100 },  // V
        { 0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010 },  // W
        { 0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001 },  // X
        { 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100 },  // Y
        { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111 },  // Z
        { 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110 },  // 0
        { 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 },  // 1
        { 0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111 },  // 2
        { 0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110 },  // 3
        { 0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010 },  // 4
        { 0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110 },  // 5
        { 0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110 },  // 6
        { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000 },  // 7
        { 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110 },  // 8
        { 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100 },  // 9
        { 0b00000, 0b00100, 0b00100, 0b00000, 0b00100, 0b00100, 0b00000 },  // : (colon)
    };

    int GetFontIndex(char c)
    {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= '0' && c <= '9') return 26 + (c - '0');
        if (c == ':') return 36;  // Colon after digits
        return -1;  // Space or unknown
    }

    void PutPixelDirect(uint8_t* base, int pitch, int x, int y,
                        gfx::Color color, int screenW, int screenH)
    {
        if (x >= 0 && x < screenW && y >= 0 && y < screenH)
        {
            gfx::Color* pixel = reinterpret_cast<gfx::Color*>(base + y * pitch) + x;
            *pixel = color;
        }
    }

    void FilledRect(gfx::Bitmap screen, int x, int y, int w, int h, gfx::Color color)
    {
        if (!gfx::BitmapLock(screen)) return;

        uint8_t* base = gfx::BitmapGetMemory(screen);
        int pitch = gfx::BitmapGetLineOffset(screen);
        int screenW = gfx::BitmapGetWidth(screen);
        int screenH = gfx::BitmapGetHeight(screen);

        // Clamp to screen bounds
        int x1 = (x < 0) ? 0 : x;
        int y1 = (y < 0) ? 0 : y;
        int x2 = (x + w > screenW) ? screenW : x + w;
        int y2 = (y + h > screenH) ? screenH : y + h;

        for (int row = y1; row < y2; ++row)
        {
            gfx::Color* pixel = reinterpret_cast<gfx::Color*>(base + row * pitch) + x1;
            for (int col = x1; col < x2; ++col)
            {
                *pixel++ = color;
            }
        }

        gfx::BitmapUnlock(screen);
    }

    void RectBorder(gfx::Bitmap screen, int x, int y, int w, int h,
                    gfx::Color color, int thickness)
    {
        FilledRect(screen, x, y, w, thickness, color);                    // Top
        FilledRect(screen, x, y + h - thickness, w, thickness, color);    // Bottom
        FilledRect(screen, x, y, thickness, h, color);                    // Left
        FilledRect(screen, x + w - thickness, y, thickness, h, color);    // Right
    }

    void StoneButton(gfx::Bitmap screen, int x, int y, int w, int h, bool selected)
    {
        if (!gfx::BitmapLock(screen)) return;

        uint8_t* base = gfx::BitmapGetMemory(screen);
        int pitch = gfx::BitmapGetLineOffset(screen);
        int screenW = gfx::BitmapGetWidth(screen);
        int screenH = gfx::BitmapGetHeight(screen);

        gfx::Color faceColor = selected ? menu::COLOR_BUTTON_SELECTED : menu::COLOR_BUTTON_FACE;
        gfx::Color lightColor = selected ? menu::COLOR_BUTTON_SEL_LIGHT : menu::COLOR_BUTTON_LIGHT;
        gfx::Color darkColor = menu::COLOR_BUTTON_DARK;

        int radius = 3;  // Corner radius

        // Draw main face (excluding corners)
        for (int row = 0; row < h; ++row)
        {
            for (int col = 0; col < w; ++col)
            {
                // Skip corner pixels for rounded effect
                bool isCorner = false;
                if (row < radius && col < radius)  // Top-left
                    isCorner = (radius - row) + (radius - col) > radius + 1;
                else if (row < radius && col >= w - radius)  // Top-right
                    isCorner = (radius - row) + (col - (w - radius - 1)) > radius + 1;
                else if (row >= h - radius && col < radius)  // Bottom-left
                    isCorner = (row - (h - radius - 1)) + (radius - col) > radius + 1;
                else if (row >= h - radius && col >= w - radius)  // Bottom-right
                    isCorner = (row - (h - radius - 1)) + (col - (w - radius - 1)) > radius + 1;

                if (!isCorner)
                    PutPixelDirect(base, pitch, x + col, y + row, faceColor, screenW, screenH);
            }
        }

        // Draw light edge (top and left) for 3D effect
        for (int col = radius; col < w - radius; ++col)
        {
            PutPixelDirect(base, pitch, x + col, y, lightColor, screenW, screenH);
            PutPixelDirect(base, pitch, x + col, y + 1, lightColor, screenW, screenH);
        }
        for (int row = radius; row < h - radius; ++row)
        {
            PutPixelDirect(base, pitch, x, y + row, lightColor, screenW, screenH);
            PutPixelDirect(base, pitch, x + 1, y + row, lightColor, screenW, screenH);
        }

        // Draw dark edge (bottom and right) for 3D effect
        for (int col = radius; col < w - radius; ++col)
        {
            PutPixelDirect(base, pitch, x + col, y + h - 1, darkColor, screenW, screenH);
            PutPixelDirect(base, pitch, x + col, y + h - 2, darkColor, screenW, screenH);
        }
        for (int row = radius; row < h - radius; ++row)
        {
            PutPixelDirect(base, pitch, x + w - 1, y + row, darkColor, screenW, screenH);
            PutPixelDirect(base, pitch, x + w - 2, y + row, darkColor, screenW, screenH);
        }

        gfx::BitmapUnlock(screen);
    }

    void Text(gfx::Bitmap screen, int x, int y, const char* text,
              gfx::Color color, int scale)
    {
        if (!gfx::BitmapLock(screen)) return;

        uint8_t* base = gfx::BitmapGetMemory(screen);
        int pitch = gfx::BitmapGetLineOffset(screen);
        int screenW = gfx::BitmapGetWidth(screen);
        int screenH = gfx::BitmapGetHeight(screen);

        int cursorX = x;
        for (const char* p = text; *p; ++p)
        {
            if (*p == ' ') {
                cursorX += 6 * scale;
                continue;
            }

            int idx = GetFontIndex(*p);
            if (idx >= 0)
            {
                for (int row = 0; row < 7; ++row)
                {
                    uint8_t rowData = FONT_5X7[idx][row];
                    for (int col = 0; col < 5; ++col)
                    {
                        if (rowData & (0b10000 >> col))
                        {
                            for (int sy = 0; sy < scale; ++sy)
                                for (int sx = 0; sx < scale; ++sx)
                                    PutPixelDirect(base, pitch, cursorX + col * scale + sx,
                                                   y + row * scale + sy, color, screenW, screenH);
                        }
                    }
                }
            }
            cursorX += 6 * scale;
        }

        gfx::BitmapUnlock(screen);
    }

    void Arrow(gfx::Bitmap screen, int x, int y, gfx::Color color)
    {
        if (!gfx::BitmapLock(screen)) return;

        uint8_t* base = gfx::BitmapGetMemory(screen);
        int pitch = gfx::BitmapGetLineOffset(screen);
        int screenW = gfx::BitmapGetWidth(screen);
        int screenH = gfx::BitmapGetHeight(screen);

        // Arrow is 10 pixels tall for better visibility
        int arrowHeight = 10;
        for (int row = 0; row < arrowHeight; ++row)
        {
            int width = (row < arrowHeight / 2) ? (row + 1) : (arrowHeight - row);
            for (int col = 0; col < width; ++col)
            {
                PutPixelDirect(base, pitch, x + col, y + row, color, screenW, screenH);
            }
        }

        gfx::BitmapUnlock(screen);
    }
}
