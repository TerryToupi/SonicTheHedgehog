#include "Scenes/GameScene.h"
#include "Scenes/SceneManager.h"
#include "Rendering/Renderer.h"
#include "Core/Input.h"
#include "Core/SystemClock.h"
#include "Core/LatelyDestroyable.h"
#include "Sprites/Ring.h"
#include "Animations/AnimatorManager.h"

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

void GameScene::Initialize()
{
    // Start camera 256px from bottom (where first tile row is)
    int vpHeight = SceneManager::Get().GetViewportHeight();
    m_CameraX = 0;
    m_CameraY = LEVEL_HEIGHT - vpHeight - 256;

    m_ShowGrid = false;
    m_ShouldExit = false;
}

void GameScene::Load()
{
    // Load level bitmap (using member loader so bitmaps persist)
    m_LevelMap = m_Loader.Load(std::string(ASSETS) + "/Textures/sonic_level.png");

    // m_LevelMap may be null if the file doesn't exist

    // Configure and load GridMap
    scene::GridConfig config;
    config.totalCols = 2560 / 4;  // 640 tile columns
    config.totalRows = 320 / 4;   // 80 tile rows
    config.tileWidth = 16;
    config.tileHeight = 16;
    config.gridElementWidth = 4;
    config.gridElementHeight = 4;
    m_Grid.Configure(config);

    std::ifstream gridFile(std::string(ASSETS) + "/Terrain/grid.csv");
    if (gridFile.is_open())
    {
        std::stringstream buffer;
        buffer << gridFile.rdbuf();
        gridFile.close();

        m_Grid.LoadFromCSV(buffer.str());
    }

    // Setup clipper for sprite rendering
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();
    m_Clipper.SetView([this, vpW, vpH]() -> const Rect& {
        static Rect viewRect;
        viewRect = { m_CameraX, m_CameraY, vpW, vpH };
        return viewRect;
    });

    // Create test rings (positioned in the visible area)
    // Camera starts at y=1056, viewport height=224, so visible y range is 1056-1280
    m_Rings.push_back(new Ring(100, 1150));
    m_Rings.push_back(new Ring(120, 1150));
    m_Rings.push_back(new Ring(140, 1150));

    // Start all ring animations
    core::SystemClock::Get().SetCurrTime();
    for (auto* ring : m_Rings)
    {
        ring->StartAnimation();
    }

    // Subscribe to events
    m_CloseHandle = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT,
        [this]() { HandleCloseEvent(); });

    m_KeyHandle = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
        [this](io::Key key) { HandleKeyEvent(key); });

    // Configure game loop
    m_Game.SetRenderLoop([this]() { OnRender(); });
    m_Game.SetInputLoop([this]() { OnInput(); });
    m_Game.SetAnimationLoop([]() {
        core::SystemClock::Get().SetCurrTime();
        anim::AnimatorManager::Get().Progress(core::SystemClock::Get().GetCurrTime());
    });
    m_Game.SetFinishingFunc([this]() { return !m_ShouldExit; });
}

void GameScene::Clear()
{
    // Destroy sprites using the engine's destruction system
    for (auto* ring : m_Rings)
    {
        ring->Destroy();
    }
    m_Rings.clear();

    // Commit destruction to actually delete the objects
    core::DestructionManager::Get().Commit();

    // Explicitly reset event handles to unsubscribe before destruction
    m_CloseHandle = core::EventHandle();
    m_KeyHandle = core::EventHandle();
}

void GameScene::OnRender()
{
    gfx::Bitmap screen = gfx::GetScreenBuffer();
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();

    // Clear the screen buffer before drawing
    gfx::BitmapClear(screen, gfx::MakeColor(0, 0, 0, 255));

    // Blit only the viewport region from the level
    if (m_LevelMap)
    {
        gfx::BitmapBlit(m_LevelMap, {m_CameraX, m_CameraY, vpW, vpH}, screen, {0, 0});
    }

    // Render rings (visibility is managed by the ring's animation callbacks)
    Rect viewArea = { 0, 0, vpW, vpH };  // Screen destination, not world coords
    for (auto* ring : m_Rings)
    {
        if (ring->IsVisible())
        {
            ring->Display(screen, viewArea, m_Clipper);
        }
    }

    // Draw grid overlay if enabled (optimized with batch Lock/Unlock)
    if (m_ShowGrid)
    {
        gfx::Color gridColor = gfx::MakeColor(255, 255, 0, 255);

        // Lock ONCE before the entire loop for performance
        if (gfx::BitmapLock(screen))
        {
            uint8_t* base = gfx::BitmapGetMemory(screen);
            int pitch = gfx::BitmapGetLineOffset(screen);

            // Calculate visible grid cell range (each cell is 4x4 pixels)
            // Apply GRID_Y_OFFSET to convert between grid coords and world coords
            Dim startCol = m_CameraX / 4;
            Dim startRow = (m_CameraY - GRID_Y_OFFSET) / 4;
            Dim endCol = (m_CameraX + vpW) / 4;
            Dim endRow = (m_CameraY - GRID_Y_OFFSET + vpH) / 4;

            // Clamp to grid bounds (grid is only 2560x320 = 10240x1280 pixels)
            if (startRow < 0) startRow = 0;
            if (endCol >= 2560) endCol = 2559;
            if (endRow >= 320) endRow = 319;

            // Draw only visible cells using direct memory writes
            for (Dim row = startRow; row <= endRow; ++row)
            {
                for (Dim col = startCol; col <= endCol; ++col)
                {
                    GridIndex value = m_Grid.GetGridTile(col, row);

                    if (value != scene::GRID_EMPTY_TILE)
                    {
                        // Convert grid coords to world coords (with offset)
                        int worldX = col * 4;
                        int worldY = row * 4 + GRID_Y_OFFSET;
                        int screenX = worldX - m_CameraX + 1;  // +1 to center in 4x4 cell
                        int screenY = worldY - m_CameraY + 1;

                        // Direct memory write instead of PutPixel (2x2 block)
                        if (screenX >= 0 && screenX + 1 < vpW &&
                            screenY >= 0 && screenY + 1 < vpH)
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
            gfx::BitmapUnlock(screen);
        }
    }

    gfx::Flush();

    // Cap at ~60 FPS
    std::this_thread::sleep_for(std::chrono::milliseconds(12));
}

void GameScene::OnInput()
{
    core::Input::UpdateInputEvents();

    // Poll keyboard state for smooth viewport movement
    if (core::Input::IsKeyPressed(io::Key::W))
    {
        m_CameraY -= SCROLL_SPEED;
        ClampCamera();
    }
    if (core::Input::IsKeyPressed(io::Key::S))
    {
        m_CameraY += SCROLL_SPEED;
        ClampCamera();
    }
    if (core::Input::IsKeyPressed(io::Key::A))
    {
        m_CameraX -= SCROLL_SPEED;
        ClampCamera();
    }
    if (core::Input::IsKeyPressed(io::Key::D))
    {
        m_CameraX += SCROLL_SPEED;
        ClampCamera();
    }
}

void GameScene::HandleKeyEvent(io::Key key)
{
    if (m_ShouldExit) return;  // Already transitioning

    if (key == io::Key::G)
    {
        m_ShowGrid = !m_ShowGrid;
    }
    else if (key == io::Key::C)
    {
        // Test: Collect the first uncollected ring
        for (auto* ring : m_Rings)
        {
            if (!ring->IsCollected())
            {
                ring->OnCollected();
                break;
            }
        }
    }
    else if (key == io::Key::Escape)
    {
        m_ShouldExit = true;
        SceneManager::Get().ChangeScene(SceneType::MENU);
    }
}

void GameScene::HandleCloseEvent()
{
    if (m_ShouldExit) return;  // Already transitioning
    m_ShouldExit = true;
    SceneManager::Get().ChangeScene(SceneType::EXIT);
}

void GameScene::ClampCamera()
{
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();

    if (m_CameraX < 0) m_CameraX = 0;
    if (m_CameraY < 0) m_CameraY = 0;
    if (m_CameraX + vpW > LEVEL_WIDTH) m_CameraX = LEVEL_WIDTH - vpW;
    if (m_CameraY + vpH > LEVEL_HEIGHT) m_CameraY = LEVEL_HEIGHT - vpH;
}
