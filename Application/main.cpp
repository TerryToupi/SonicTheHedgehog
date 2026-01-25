#include "Rendering/Renderer.h"
#include "Scene/GridLayer.h"
#include "Core/EventRegistry.h"
#include "Core/Input.h"
#include "IO/IOMapping.h"
#include "Sound/Sound.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

// Game state machine
enum class GameState { MENU, GAME, CREDITS };

// Menu button configuration
struct Button {
	int x, y, width, height;
};

// Button colors (Stone-like Sonic aesthetic)
constexpr gfx::Color COLOR_BUTTON_FACE = 0xFF808080;       // Grey stone face
constexpr gfx::Color COLOR_BUTTON_LIGHT = 0xFFC0C0C0;      // Light grey highlight (top/left)
constexpr gfx::Color COLOR_BUTTON_DARK = 0xFF404040;       // Dark grey shadow (bottom/right)
constexpr gfx::Color COLOR_BUTTON_SELECTED = 0xFFA0A0A0;   // Lighter grey when selected
constexpr gfx::Color COLOR_BUTTON_SEL_LIGHT = 0xFFE0E0E0;  // Brighter highlight when selected
constexpr gfx::Color COLOR_TEXT = 0xFF000000;              // Black text
constexpr gfx::Color COLOR_TEXT_SHADOW = 0xFF303030;       // Dark shadow for text
constexpr gfx::Color COLOR_ARROW = 0xFF00D0FF;             // Cyan arrow (Sonic blue)

// Helper to put a single pixel (no lock/unlock - caller must handle)
inline void PutPixelDirect(uint8_t* base, int pitch, int x, int y, gfx::Color color, int screenW, int screenH)
{
	if (x >= 0 && x < screenW && y >= 0 && y < screenH)
	{
		gfx::Color* pixel = reinterpret_cast<gfx::Color*>(base + y * pitch) + x;
		*pixel = color;
	}
}

// Helper function to draw a filled rectangle
void DrawFilledRect(gfx::Bitmap screen, int x, int y, int w, int h, gfx::Color color)
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

// Helper function to draw a rectangle border
void DrawRectBorder(gfx::Bitmap screen, int x, int y, int w, int h, gfx::Color color, int thickness = 2)
{
	DrawFilledRect(screen, x, y, w, thickness, color);           // Top
	DrawFilledRect(screen, x, y + h - thickness, w, thickness, color);  // Bottom
	DrawFilledRect(screen, x, y, thickness, h, color);           // Left
	DrawFilledRect(screen, x + w - thickness, y, thickness, h, color);  // Right
}

// Draw a 3D stone-like button with rounded corners
void DrawStoneButton(gfx::Bitmap screen, int x, int y, int w, int h, bool selected)
{
	if (!gfx::BitmapLock(screen)) return;

	uint8_t* base = gfx::BitmapGetMemory(screen);
	int pitch = gfx::BitmapGetLineOffset(screen);
	int screenW = gfx::BitmapGetWidth(screen);
	int screenH = gfx::BitmapGetHeight(screen);

	gfx::Color faceColor = selected ? COLOR_BUTTON_SELECTED : COLOR_BUTTON_FACE;
	gfx::Color lightColor = selected ? COLOR_BUTTON_SEL_LIGHT : COLOR_BUTTON_LIGHT;
	gfx::Color darkColor = COLOR_BUTTON_DARK;

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

// Simple 5x7 pixel font for text (A-Z and 0-9)
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
};

// Map character to font index
int GetFontIndex(char c)
{
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= '0' && c <= '9') return 26 + (c - '0');
	return -1;  // Space or unknown
}

// Draw text using the simple pixel font
void DrawText(gfx::Bitmap screen, int x, int y, const char* text, gfx::Color color, int scale = 1)
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

// Helper function to draw selection arrow (pointing right)
void DrawArrow(gfx::Bitmap screen, int x, int y, gfx::Color color)
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

// Helper to check if point is inside button
bool IsPointInButton(int px, int py, const Button& btn)
{
	return px >= btn.x && px < btn.x + btn.width &&
	       py >= btn.y && py < btn.y + btn.height;
}

int main(void)
{
	bool close = false;
	bool showGrid = false;

	// Menu state
	GameState currentState = GameState::MENU;
	int selectedButton = 0;  // 0 = Play, 1 = Credits
	int mouseX = 0, mouseY = 0;

	// Camera/viewport configuration (320x224 = classic Sega Genesis resolution)
	const int VIEWPORT_WIDTH = 320;
	const int VIEWPORT_HEIGHT = 224;
	const int LEVEL_WIDTH = 10240;
	const int LEVEL_HEIGHT = 1536;
	const int SCROLL_SPEED = 1;  // pixels per keypress (scaled for 320x224 viewport)
	const int GRID_Y_OFFSET = 256;  // Grid offset to align with level graphics

	// Initialize camera 256px from bottom (where first tile row is)
	int cameraX = 0;
	int cameraY = LEVEL_HEIGHT - VIEWPORT_HEIGHT - 256;  // 1056

	gfx::Open("Sonic Level Viewer", 1280, 720);
	gfx::SetScreenBuffer(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);  // Classic resolution, scaled up to window

	sound::Open();

	// Load the sonic level map
	gfx::BitmapLoader loader;
	gfx::Bitmap levelMap = loader.Load(std::string(ASSETS) + "/Textures/sonic_level.png");

	if (!levelMap)
	{
		std::cerr << "Failed to load sonic_level.png" << std::endl;
		return -1;
	}

	// Load menu background
	gfx::Bitmap menuBackground = loader.Load(std::string(ASSETS) + "/Textures/sonic_menu.png");
	if (!menuBackground)
	{
		std::cerr << "Failed to load sonic_menu.png" << std::endl;
		return -1;
	}

	// Menu button configuration (centered horizontally in 320x224 viewport)
	const int BUTTON_WIDTH = 80;
	const int BUTTON_HEIGHT = 20;
	const int BUTTON_X = (VIEWPORT_WIDTH - BUTTON_WIDTH) / 2;  // 120
	const int PLAY_BUTTON_Y = 155;
	const int CREDITS_BUTTON_Y = 185;
	const int ARROW_OFFSET = 12;  // Distance from arrow to button

	Button buttons[2] = {
		{ BUTTON_X, PLAY_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT },     // Play
		{ BUTTON_X, CREDITS_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT }   // Credits
	};

	// Configure GridMap
	// Grid CSV: 2560 columns × 320 rows at 4×4 pixels = 10240×1280 pixels coverage
	scene::GridConfig gridConfig;
	gridConfig.totalCols = 2560 / 4;  // 640 tile columns
	gridConfig.totalRows = 320 / 4;   // 80 tile rows
	gridConfig.tileWidth = 16;
	gridConfig.tileHeight = 16;
	gridConfig.gridElementWidth = 4;
	gridConfig.gridElementHeight = 4;

	scene::GridMap grid;
	grid.Configure(gridConfig);

	// Load grid from CSV
	std::ifstream gridFile(std::string(ASSETS) + "/Terrain/grid.csv");
	if (!gridFile.is_open())
	{
		std::cerr << "Failed to open grid.csv" << std::endl;
		return -1;
	}

	std::stringstream buffer;
	buffer << gridFile.rdbuf();
	gridFile.close();

	if (!grid.LoadFromCSV(buffer.str()))
	{
		std::cerr << "Failed to load grid from CSV" << std::endl;
		return -1;
	}

	std::cout << "Grid loaded successfully!" << std::endl;
	std::cout << "Controls:" << std::endl;
	std::cout << "  G - Toggle grid visualization" << std::endl;
	std::cout << "  WASD - Move camera (viewport)" << std::endl;
	std::cout << "Starting at bottom-left of level (0, 816)" << std::endl;

	// Helper to keep camera within level bounds
	auto clampCamera = [&]() {
		if (cameraX < 0) cameraX = 0;
		if (cameraY < 0) cameraY = 0;
		if (cameraX + VIEWPORT_WIDTH > LEVEL_WIDTH)
			cameraX = LEVEL_WIDTH - VIEWPORT_WIDTH;
		if (cameraY + VIEWPORT_HEIGHT > LEVEL_HEIGHT)
			cameraY = LEVEL_HEIGHT - VIEWPORT_HEIGHT;
	};

	// Event subscriptions for one-shot events
	auto h1 = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT,
		[&close]() {
			close = true;
			std::cout << "Application closing!" << std::endl;
		});

	auto h2 = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
		[&](io::Key key) {
			if (currentState == GameState::MENU)
			{
				// Menu navigation (WASD like the game)
				if (key == io::Key::W)
				{
					selectedButton = (selectedButton > 0) ? selectedButton - 1 : 1;
				}
				else if (key == io::Key::S)
				{
					selectedButton = (selectedButton < 1) ? selectedButton + 1 : 0;
				}
				else if (key == io::Key::Space || key == io::Key::Enter)
				{
					if (selectedButton == 0)
					{
						currentState = GameState::GAME;
						std::cout << "Starting game..." << std::endl;
					}
					else
					{
						currentState = GameState::CREDITS;
						std::cout << "Showing credits..." << std::endl;
					}
				}
			}
			else if (currentState == GameState::GAME)
			{
				// Game controls
				if (key == io::Key::G)
				{
					showGrid = !showGrid;
					std::cout << "Grid display: " << (showGrid ? "ON" : "OFF") << std::endl;
				}
				else if (key == io::Key::Escape)
				{
					currentState = GameState::MENU;
					std::cout << "Returning to menu..." << std::endl;
				}
			}
			else if (currentState == GameState::CREDITS)
			{
				// Any key returns to menu
				currentState = GameState::MENU;
				std::cout << "Returning to menu..." << std::endl;
			}
		});

	// Mouse motion for menu hover detection
	auto h3 = core::EventRegistry::Subscribe(EventType::MOUSE_MORION_EVENT,
		[&](int x, int y) {
			// Convert window coordinates to viewport coordinates
			// Window is 1280x720, viewport is 320x224
			mouseX = x * VIEWPORT_WIDTH / 1280;
			mouseY = y * VIEWPORT_HEIGHT / 720;

			if (currentState == GameState::MENU)
			{
				// Update selection based on mouse hover
				for (int i = 0; i < 2; ++i)
				{
					if (IsPointInButton(mouseX, mouseY, buttons[i]))
					{
						selectedButton = i;
						break;
					}
				}
			}
		});

	// Mouse button for menu selection
	auto h4 = core::EventRegistry::Subscribe(EventType::MOUSE_BUTTON_EVENT,
		[&](io::Button button) {
			if (button == io::Button::Left)
			{
				if (currentState == GameState::MENU)
				{
					for (int i = 0; i < 2; ++i)
					{
						if (IsPointInButton(mouseX, mouseY, buttons[i]))
						{
							if (i == 0)
							{
								currentState = GameState::GAME;
								std::cout << "Starting game..." << std::endl;
							}
							else
							{
								currentState = GameState::CREDITS;
								std::cout << "Showing credits..." << std::endl;
							}
							break;
						}
					}
				}
				else if (currentState == GameState::CREDITS)
				{
					currentState = GameState::MENU;
					std::cout << "Returning to menu..." << std::endl;
				}
			}
		});

	std::string sfx_path(ASSETS);
	sfx_path += "/Sounds/a.mp3";
	sound::SFX sfx = sound::LoadSFX(sfx_path.c_str());

	auto h5 = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
		[&sfx](io::Key key) {
			if (key == io::Key::P)
			{
				sound::PlaySFX(sfx);
			}
		});

	while (!close)
	{
		core::Input::UpdateInputEvents();
		gfx::Bitmap screenBuf = gfx::GetScreenBuffer();

		switch (currentState)
		{
		case GameState::MENU:
		{
			// Clear screen
			gfx::BitmapClear(screenBuf, gfx::MakeColor(0, 0, 0, 255));

			// Draw menu background (scaled to fit viewport)
			int bgWidth = gfx::BitmapGetWidth(menuBackground);
			int bgHeight = gfx::BitmapGetHeight(menuBackground);
			gfx::BitmapBlitScaled(
				menuBackground,
				{ 0, 0, bgWidth, bgHeight },
				screenBuf,
				{ 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT }
			);

			// Draw buttons with stone-like appearance
			const char* buttonLabels[2] = { "PLAY", "CREDITS" };
			for (int i = 0; i < 2; ++i)
			{
				bool isSelected = (i == selectedButton);

				// Draw stone button
				DrawStoneButton(screenBuf, buttons[i].x, buttons[i].y,
				                buttons[i].width, buttons[i].height, isSelected);

				// Calculate text position (centered in button)
				int textLen = (i == 0) ? 4 : 7;  // "PLAY" = 4, "CREDITS" = 7
				int textWidth = textLen * 6 - 1;  // 5 pixels per char + 1 spacing
				int textX = buttons[i].x + (buttons[i].width - textWidth) / 2;
				int textY = buttons[i].y + (buttons[i].height - 7) / 2;

				// Draw text shadow then text
				DrawText(screenBuf, textX + 1, textY + 1, buttonLabels[i], COLOR_TEXT_SHADOW);
				DrawText(screenBuf, textX, textY, buttonLabels[i], COLOR_TEXT);

				// Draw selection arrow if selected
				if (isSelected)
				{
					int arrowX = buttons[i].x - ARROW_OFFSET - 2;
					int arrowY = buttons[i].y + (buttons[i].height - 10) / 2;
					DrawArrow(screenBuf, arrowX, arrowY, COLOR_ARROW);
				}
			}
			break;
		}

		case GameState::GAME:
		{
			// Poll keyboard state for smooth viewport movement
			if (core::Input::IsKeyPressed(io::Key::W))
			{
				cameraY -= SCROLL_SPEED;
				clampCamera();
			}
			if (core::Input::IsKeyPressed(io::Key::S))
			{
				cameraY += SCROLL_SPEED;
				clampCamera();
			}
			if (core::Input::IsKeyPressed(io::Key::A))
			{
				cameraX -= SCROLL_SPEED;
				clampCamera();
			}
			if (core::Input::IsKeyPressed(io::Key::D))
			{
				cameraX += SCROLL_SPEED;
				clampCamera();
			}

			// Clear the screen buffer before drawing
			gfx::BitmapClear(screenBuf, gfx::MakeColor(0, 0, 0, 255));

			// Blit only the viewport region from the level
			gfx::BitmapBlit(
				levelMap,
				{ cameraX, cameraY, VIEWPORT_WIDTH, VIEWPORT_HEIGHT },
				screenBuf,
				{ 0, 0 }
			);

			// Draw grid overlay if enabled (optimized with batch Lock/Unlock)
			if (showGrid)
			{
				gfx::Color gridColor = gfx::MakeColor(255, 255, 0, 255);

				// Lock ONCE before the entire loop for performance
				if (gfx::BitmapLock(screenBuf))
				{
					uint8_t* base = gfx::BitmapGetMemory(screenBuf);
					int pitch = gfx::BitmapGetLineOffset(screenBuf);

					// Calculate visible grid cell range (each cell is 4x4 pixels)
					// Apply GRID_Y_OFFSET to convert between grid coords and world coords
					Dim startCol = cameraX / 4;
					Dim startRow = (cameraY - GRID_Y_OFFSET) / 4;
					Dim endCol = (cameraX + VIEWPORT_WIDTH) / 4;
					Dim endRow = (cameraY - GRID_Y_OFFSET + VIEWPORT_HEIGHT) / 4;

					// Clamp to grid bounds (grid is only 2560x320 = 10240x1280 pixels)
					if (startRow < 0) startRow = 0;
					if (endCol >= 2560) endCol = 2559;
					if (endRow >= 320) endRow = 319;

					// Draw only visible cells using direct memory writes
					for (Dim row = startRow; row <= endRow; ++row)
					{
						for (Dim col = startCol; col <= endCol; ++col)
						{
							GridIndex value = grid.GetGridTile(col, row);

							if (value != scene::GRID_EMPTY_TILE)
							{
								// Convert grid coords to world coords (with offset)
								int worldX = col * 4;
								int worldY = row * 4 + GRID_Y_OFFSET;
								int screenX = worldX - cameraX + 1;  // +1 to center in 4x4 cell
								int screenY = worldY - cameraY + 1;

								// Direct memory write instead of PutPixel (2x2 block)
								if (screenX >= 0 && screenX + 1 < VIEWPORT_WIDTH &&
									screenY >= 0 && screenY + 1 < VIEWPORT_HEIGHT)
								{
									gfx::Color* pixel = reinterpret_cast<gfx::Color*>(base + screenY * pitch) + screenX;
									pixel[0] = gridColor;
									pixel[1] = gridColor;
									pixel = reinterpret_cast<gfx::Color*>(base + (screenY + 1) * pitch) + screenX;
									pixel[0] = gridColor;
									pixel[1] = gridColor;
								}
							}
						}
					}

					// Unlock ONCE after the entire loop
					gfx::BitmapUnlock(screenBuf);
				}
			}
			break;
		}

		case GameState::CREDITS:
		{
			// Credits screen - dark blue background
			gfx::BitmapClear(screenBuf, gfx::MakeColor(0, 0, 64, 255));

			// Draw content area
			int contentX = 20;
			int contentY = 30;
			int contentW = VIEWPORT_WIDTH - 40;
			int contentH = VIEWPORT_HEIGHT - 60;
			DrawFilledRect(screenBuf, contentX, contentY, contentW, contentH,
			               gfx::MakeColor(0, 0, 96, 255));
			DrawRectBorder(screenBuf, contentX, contentY, contentW, contentH,
			               gfx::MakeColor(100, 100, 200, 255), 2);

			// Draw "CREDITS" title
			gfx::Color titleColor = gfx::MakeColor(255, 215, 0, 255);  // Gold
			gfx::Color textColor = gfx::MakeColor(255, 255, 255, 255);  // White
			gfx::Color shadowColor = gfx::MakeColor(0, 0, 40, 255);

			DrawText(screenBuf, 121, 45, "CREDITS", shadowColor);
			DrawText(screenBuf, 120, 44, "CREDITS", titleColor);

			// Draw names
			// Lefteris Toupis, CSDP1457
			DrawText(screenBuf, 71, 80, "LEFTERIS TOUPIS", shadowColor);
			DrawText(screenBuf, 70, 79, "LEFTERIS TOUPIS", textColor);
			DrawText(screenBuf, 111, 95, "CSDP1457", shadowColor);
			DrawText(screenBuf, 110, 94, "CSDP1457", textColor);

			// Mike Giannakopoulos, CSDP1464
			DrawText(screenBuf, 41, 125, "MIKE GIANNAKOPOULOS", shadowColor);
			DrawText(screenBuf, 40, 124, "MIKE GIANNAKOPOULOS", textColor);
			DrawText(screenBuf, 111, 140, "CSDP1464", shadowColor);
			DrawText(screenBuf, 110, 139, "CSDP1464", textColor);

			// Draw hint at the bottom
			static int flashCounter = 0;
			flashCounter = (flashCounter + 1) % 60;
			if (flashCounter < 30)
			{
				gfx::Color hintColor = gfx::MakeColor(180, 180, 255, 255);
				DrawText(screenBuf, 68, 185, "PRESS ANY KEY", hintColor);
			}
			break;
		}
		}

		gfx::Flush();

		// Cap at ~60 FPS to make scroll speed consistent
		std::this_thread::sleep_for(std::chrono::milliseconds(12));
	}

	sound::DestroySFX(sfx);

	sound::Close();
	gfx::Close();

	return 0;
}
