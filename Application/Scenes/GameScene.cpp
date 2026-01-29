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

#include <nlohmann/json.hpp>

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
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();

    // Load parallax background
    m_ParallaxBackground = m_Loader.Load(std::string(ASSETS) + "/Textures/background.png");

    // Load tileset and configure TileLayer
    gfx::Bitmap tileset = m_Loader.Load(std::string(ASSETS) + "/Textures/tiles_first_map_fixed.png");

    scene::TileConfig tileConfig;
    tileConfig.totalCols = 40;
    tileConfig.totalRows = 6;
    tileConfig.tileWidth = 256;
    tileConfig.tileHeight = 256;
    tileConfig.viewWindow = {0, 0, vpW, vpH};
    tileConfig.tilesetCols = 5;
    tileConfig.tilesetOffsetX = 8;
    tileConfig.tilesetOffsetY = 8;
    tileConfig.tilesetMarginX = 8;
    tileConfig.tilesetMarginY = 8;

    m_TileLayer.Configure(tileConfig);
    m_TileLayer.SetTileset(tileset);

    // Load tile map
    std::ifstream tileFile(std::string(ASSETS) + "/Terrain/sonic_level.csv");
    if (tileFile.is_open())
    {
        std::stringstream buffer;
        buffer << tileFile.rdbuf();
        tileFile.close();
        m_TileLayer.LoadFromCSV(buffer.str());
    }

    // Configure and load GridMap
    // Using 1x1 pixel grid for precise collision detection (especially for ramps)
    // Note: totalCols/totalRows are in TILE units (16x16), grid internally expands by GridElementsPerTile
    scene::GridConfig config;
    config.totalCols = 10240 / 16;  // 640 tiles
    config.totalRows = 1536 / 16;   // 96 tiles
    config.tileWidth = 16;
    config.tileHeight = 16;
    config.gridElementWidth = 1;    // 1 pixel per grid element (16x16 = 256 elements per tile)
    config.gridElementHeight = 1;
    config.SlopMaxElevetionPx = 7;  // Allow smooth slope walking, require jump for larger height differences
    m_Grid.Configure(config);

    // Try to load RLE format first (fast), fall back to CSV and auto-convert
    std::string rlePath = std::string(ASSETS) + "/Terrain/grid_1x1.grle";
    std::string csvPath = std::string(ASSETS) + "/Terrain/grid_1x1.csv";

    if (!m_Grid.LoadFromRLE(rlePath))
    {
        // RLE not found or invalid, load from CSV
        std::ifstream gridFile(csvPath);
        if (gridFile.is_open())
        {
            std::stringstream buffer;
            buffer << gridFile.rdbuf();
            gridFile.close();
            m_Grid.LoadFromCSV(buffer.str());

            // Auto-save as RLE for faster loading next time
            m_Grid.SaveToRLE(rlePath);
        }
    }

    // Setup clipper for sprite rendering
    m_Clipper.SetView([this, vpW, vpH]() -> const Rect& {
        static Rect viewRect;
        viewRect = { m_CameraX, m_CameraY, vpW, vpH };
        return viewRect;
    });

    m_Rings.push_back(new Ring(100, 1150));
    m_Rings.push_back(new Ring(120, 1150));
    m_Rings.push_back(new Ring(140, 1150));

    // Start all ring animations
    core::SystemClock::Get().SetCurrTime();
    for (auto* ring : m_Rings)
    {
        ring->StartAnimation();
    }

    // Load and start flower animations
    LoadFlowers();
    for (auto* flower : m_Flowers)
    {
        flower->StartAnimation();
    }

    // Create test checkpoint (positioned near the rings)
    m_Checkpoint = new Checkpoint(410, 1170);

    // Create Sonic (positioned in visible area)
    m_Sonic = new Sonic(50, 1100, &m_Grid, &m_TileLayer);

    // Register collision pairs between Sonic and collectibles
    RegisterCollisions();

    // Subscribe to events
    m_CloseHandle = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT,
        [this]() { HandleCloseEvent(); });

    m_KeyHandle = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
        [this](io::Key key) { HandleKeyEvent(key); });

    // Load and play background music on infinite loop
    m_BackgroundMusic = sound::LoadTrack((std::string(ASSETS) + "/Sounds/Sonic The Hedgehog OST - Green Hill Zone.mp3").c_str());
    sound::PlayTrack(m_BackgroundMusic, -1);  // -1 for infinite looping

    // Configure game loop
    m_Game.SetRenderLoop([this]() { OnRender(); });
    m_Game.SetInputLoop([this]() { OnInput(); });
    m_Game.SetAnimationLoop([]() {
        core::SystemClock::Get().SetCurrTime();
        anim::AnimatorManager::Get().Progress(core::SystemClock::Get().GetCurrTime());
    });
    m_Game.SetFinishingFunc([this]() { return !m_ShouldExit; });
    m_Game.SetCollisionsCheckingLoop([this]() { OnCollisionCheckLoop(); });
}

void GameScene::Clear()
{
    // Stop and clean up background music
    if (m_BackgroundMusic)
    {
        sound::StopTrack(m_BackgroundMusic, 500);  // 500ms fade out
        sound::DestroyTrack(m_BackgroundMusic);
        m_BackgroundMusic = nullptr;
    }

    // Cancel collision registrations before destroying sprites
    auto& checker = physics::CollisionChecker::Get();
    for (auto* ring : m_Rings)
    {
        checker.Cancel(m_Sonic, ring);
    }
    if (m_Checkpoint)
    {
        checker.Cancel(m_Sonic, m_Checkpoint);
    }

    // Destroy sprites using the engine's destruction system
    for (auto* ring : m_Rings)
    {
        ring->Destroy();
    }
    m_Rings.clear();

    for (auto* flower : m_Flowers)
    {
        flower->Destroy();
    }
    m_Flowers.clear();

    if (m_Checkpoint)
    {
        m_Checkpoint->Destroy();
        m_Checkpoint = nullptr;
    }

    if (m_Sonic)
    {
        m_Sonic->Destroy();
        m_Sonic = nullptr;
    }

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

    // Clear the screen buffer with light blue sea color
    gfx::BitmapClear(screen, gfx::MakeColor(148, 216, 240, 255));

    // Render parallax background (scrolls at 30% of camera speed for depth effect)
    // Background: 1.5 rows sea blue at top, 4 rows background, 0.5 rows sea blue at bottom
    if (m_ParallaxBackground)
    {
        int bgW = gfx::BitmapGetWidth(m_ParallaxBackground);
        int bgH = gfx::BitmapGetHeight(m_ParallaxBackground);

        // Background world position: 1.5 rows from top, 4 rows height, 0.5 rows at bottom
        constexpr int BG_WORLD_Y = 384;       // Start at 1.5 rows (1.5 × 256)
        constexpr int BG_WORLD_HEIGHT = 1024; // 4 rows × 256 pixels

        // Scale background to fit the 4-row height while maintaining aspect ratio
        float scale = static_cast<float>(BG_WORLD_HEIGHT) / bgH;
        int scaledW = static_cast<int>(bgW * scale);
        int scaledH = BG_WORLD_HEIGHT;

        // Calculate screen Y position based on camera
        int bgScreenY = BG_WORLD_Y - m_CameraY;

        // Only render if background is visible on screen
        if (bgScreenY < vpH && bgScreenY + scaledH > 0)
        {
            // Parallax scroll at 30% of camera speed
            int parallaxX = (m_CameraX * 30) / 100;

            // Scale the parallax offset to match the scaled background width
            int scaledParallaxX = static_cast<int>(parallaxX * scale);

            // Calculate tile offset for seamless horizontal wrapping
            int tileOffset = scaledParallaxX % scaledW;
            if (tileOffset < 0) tileOffset += scaledW;

            // Render tiled background at the correct Y position
            int destX = -tileOffset;
            while (destX < vpW)
            {
                gfx::BitmapBlitScaled(
                    m_ParallaxBackground,
                    {0, 0, bgW, bgH},
                    screen,
                    {destX, bgScreenY, scaledW, scaledH}
                );
                destX += scaledW;
            }
        }
    }

    // Render tile-based level
    m_TileLayer.SetViewWindow({m_CameraX, m_CameraY, vpW, vpH});
    m_TileLayer.Display(screen, {0, 0});

    // Render flowers (behind all other sprites)
    Rect viewArea = { 0, 0, vpW, vpH };  // Screen destination, not world coords
    for (auto* flower : m_Flowers)
    {
        if (flower->IsVisible())
        {
            flower->Display(screen, viewArea, m_Clipper);
        }
    }

    // Render rings (visibility is managed by the ring's animation callbacks)
    for (auto* ring : m_Rings)
    {
        if (ring->IsVisible())
        {
            ring->Display(screen, viewArea, m_Clipper);
        }
    }

    // Render checkpoint
    if (m_Checkpoint && m_Checkpoint->IsVisible())
    {
        m_Checkpoint->Display(screen, viewArea, m_Clipper);
    }

    // Render Sonic
    if (m_Sonic && m_Sonic->IsVisible())
    {
        m_Sonic->Display(screen, viewArea, m_Clipper);
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

            // Calculate visible grid cell range (each cell is 1x1 pixel with 1x1 grid)
            // Apply GRID_Y_OFFSET to convert between grid coords and world coords
            Dim startCol = m_CameraX;
            Dim startRow = (m_CameraY - GRID_Y_OFFSET);
            Dim endCol = (m_CameraX + vpW);
            Dim endRow = (m_CameraY - GRID_Y_OFFSET + vpH);

            // Clamp to grid bounds (grid is 10240x1536 pixels = 640*16 x 96*16)
            Dim gridPixelWidth = 10240;
            Dim gridPixelHeight = 1536;
            if (startCol < 0) startCol = 0;
            if (startRow < 0) startRow = 0;
            if (endCol >= gridPixelWidth) endCol = gridPixelWidth - 1;
            if (endRow >= gridPixelHeight) endRow = gridPixelHeight - 1;

            // Draw only visible cells using direct memory writes
            for (Dim row = startRow; row <= endRow; ++row)
            {
                for (Dim col = startCol; col <= endCol; ++col)
                {
                    GridIndex value = m_Grid.GetGridTile(col, row);

                    if (value != scene::GRID_EMPTY_TILE)
                    {
                        // Convert grid coords to world coords (with offset)
                        int worldX = col;
                        int worldY = row + GRID_Y_OFFSET;
                        int screenX = worldX - m_CameraX;
                        int screenY = worldY - m_CameraY;

                        // Direct memory write - single pixel for 1x1 grid
                        if (screenX >= 0 && screenX < vpW &&
                            screenY >= 0 && screenY < vpH)
                        {
                            gfx::Color* pixel = reinterpret_cast<gfx::Color*>(base + screenY * pitch) + screenX;
                            pixel[0] = gridColor;
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

    // Update Sonic and make camera follow
    if (m_Sonic)
    {
        m_Sonic->Update();

        int vpW = SceneManager::Get().GetViewportWidth();
        int vpH = SceneManager::Get().GetViewportHeight();

        // Horizontal: camera follows Sonic directly
        m_CameraX = m_Sonic->GetCenterX() - vpW / 3;  // Sonic at ~1/3 from left

        // Vertical: camera follows the highest ground Sonic has touched
        // Update target Y when Sonic is on ground
        if (m_Sonic->IsOnGround())
        {
            m_CameraTargetY = static_cast<float>(m_Sonic->GetBottomY() - vpH * 3 / 4);
        }

        // Initialize camera on first frame to avoid sudden jump
        if (!m_CameraInitialized)
        {
            m_CameraSmoothY = static_cast<float>(m_Sonic->GetBottomY() - vpH * 3 / 4);
            m_CameraTargetY = m_CameraSmoothY;
            m_CameraInitialized = true;
        }

        // Smooth interpolation towards target Y
        constexpr float CAMERA_SMOOTH_SPEED = 0.08f;  // Adjust for faster/slower transitions
        m_CameraSmoothY += (m_CameraTargetY - m_CameraSmoothY) * CAMERA_SMOOTH_SPEED;

        // Boundary constraints: keep Sonic within 25px of screen edges
        constexpr int SCREEN_MARGIN = 25;
        int sonicScreenY = m_Sonic->GetCenterY() - static_cast<int>(m_CameraSmoothY);

        // If Sonic is too close to bottom, push camera down
        if (sonicScreenY > vpH - SCREEN_MARGIN)
        {
            m_CameraSmoothY = static_cast<float>(m_Sonic->GetCenterY() - (vpH - SCREEN_MARGIN));
        }
        // If Sonic is too close to top, push camera up
        else if (sonicScreenY < SCREEN_MARGIN)
        {
            m_CameraSmoothY = static_cast<float>(m_Sonic->GetCenterY() - SCREEN_MARGIN);
        }

        m_CameraY = static_cast<int>(m_CameraSmoothY);
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
    else if (key == io::Key::T)
    {
        // Test: Trigger the checkpoint
        if (m_Checkpoint && !m_Checkpoint->IsTriggered())
        {
            m_Checkpoint->OnTriggered();
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

void GameScene::RegisterCollisions()
{
    auto& checker = physics::CollisionChecker::Get();

    // Register Sonic vs each Ring
    for (auto* ring : m_Rings)
    {
        checker.Register(m_Sonic, ring,
            [](scene::Sprite* sonic, scene::Sprite* ringSprite) {
                Ring* ring = static_cast<Ring*>(ringSprite);
                if (!ring->IsCollected())
                {
                    ring->OnCollected();
                }
            }
        );
    }

    // Register Sonic vs Checkpoint
    if (m_Checkpoint)
    {
        checker.Register(m_Sonic, m_Checkpoint,
            [](scene::Sprite* sonic, scene::Sprite* checkpointSprite) {
                Checkpoint* checkpoint = static_cast<Checkpoint*>(checkpointSprite);
                if (!checkpoint->IsTriggered())
                {
                    checkpoint->OnTriggered();
                }
            }
        );
    }
}

void GameScene::OnCollisionCheckLoop()
{
    physics::CollisionChecker::Get().Check();
}

void GameScene::LoadFlowers()
{
    std::ifstream file(std::string(ASSETS) + "/Data/flowers.json");
    if (!file.is_open())
    {
        return;
    }

    nlohmann::json data;
    file >> data;
    file.close();

    for (const auto& flowerType : data["flowers"])
    {
        std::string filmId = flowerType["filmId"];
        for (const auto& pos : flowerType["positions"])
        {
            int x = pos[0];
            int y = pos[1];
            m_Flowers.push_back(new Flower(x, y, filmId));
        }
    }
}
