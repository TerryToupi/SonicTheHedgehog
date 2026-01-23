#include "Rendering/Renderer.h"
#include "Scene/GridLayer.h"
#include "Core/EventRegistry.h"
#include "Core/Input.h"
#include "IO/IOMapping.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

int main(void)
{
	bool close = false;
	bool showGrid = false;

	// Camera/viewport configuration (320x224 = classic Sega Genesis resolution)
	const int VIEWPORT_WIDTH = 320;
	const int VIEWPORT_HEIGHT = 224;
	const int LEVEL_WIDTH = 10240;
	const int LEVEL_HEIGHT = 1536;
	const int SCROLL_SPEED = 1;  // pixels per keypress (scaled for 320x224 viewport)
	const int GRID_Y_OFFSET = 256;  // Grid offset to align with level graphics

	// Initialize camera 256px from bottom (where first tile row is)
	int cameraX = 0;
	int cameraY = LEVEL_HEIGHT -	 VIEWPORT_HEIGHT - 256;  // 1056

	gfx::Open("Sonic Level Viewer", 1280, 720);
	gfx::SetScreenBuffer(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);  // Classic resolution, scaled up to window

	// Load the sonic level map
	gfx::BitmapLoader loader;
	gfx::Bitmap levelMap = loader.Load(std::string(ASSETS) + "/Textures/sonic_level.png");

	if (!levelMap)
	{
		std::cerr << "Failed to load sonic_level.png" << std::endl;
		return -1;
	}

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
	std::ifstream gridFile(std::string(ASSETS) + "/grid.csv");
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
			if (key == io::Key::G)
			{
				showGrid = !showGrid;
				std::cout << "Grid display: " << (showGrid ? "ON" : "OFF") << std::endl;
			}
		});

	while (!close)
	{
		core::Input::Update();

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
		gfx::BitmapClear(gfx::GetScreenBuffer(), gfx::MakeColor(0, 0, 0, 255));

		// Blit only the viewport region from the level
		gfx::BitmapBlit(
			levelMap,
			{ cameraX, cameraY, VIEWPORT_WIDTH, VIEWPORT_HEIGHT },
			gfx::GetScreenBuffer(),
			{ 0, 0 }
		);

		// Draw grid overlay if enabled (optimized with batch Lock/Unlock)
		if (showGrid)
		{
			gfx::Bitmap screenBuf = gfx::GetScreenBuffer();
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

		gfx::Flush();

		// Cap at ~60 FPS to make scroll speed consistent
		std::this_thread::sleep_for(std::chrono::milliseconds(12));
	}

	gfx::BitmapDestroy(levelMap);
	gfx::Close();

	return 0;
}
