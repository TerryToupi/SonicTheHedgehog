#include "Scenes/GameScene.h"
#include "Scenes/SceneManager.h"
#include "Scenes/TunnelPaths.h"
#include "Rendering/Renderer.h"
#include "Core/Input.h"
#include "Core/SystemClock.h"
#include "Core/LatelyDestroyable.h"
#include "Sprites/Ring.h"
#include "Animations/AnimatorManager.h"
#include "Animations/AnimationFilmHolder.h"
#include "Game/GameStats.h"
#include "Utilities/DrawHelpers.h"
#include "Utilities/MenuConstants.h"

#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include <nlohmann/json.hpp>
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Static member initialization
sound::SFX GameScene::s_RingDropSound = nullptr;

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

    // Load rings from JSON data file
    LoadRings();

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

    // Initialize tunnel paths and pass to Sonic
    m_TunnelPaths = TunnelPaths::CreateAllTunnels();
    m_Sonic->SetTunnelPaths(&m_TunnelPaths);

    // Load ring drop sound and set up ring scatter callback
    if (!s_RingDropSound)
    {
        s_RingDropSound = sound::LoadSFX(ASSETS "/Sounds/sonic-rings-drop.mp3");
    }
    m_Sonic->SetRingScatterCallback([this](int x, int y, int count) {
        SpawnScatteredRings(x, y, count);
    });

    // Create enemies - Masher at the first gap
    // Position the masher so it jumps up from a pit area
    m_Mashers.push_back(new Masher(1210, 1270));

    Masher* masher2 = new Masher(1130, 1270);
    masher2->SetJumpDelay(45);  // Offset by ~0.75 seconds so they're not synchronized
    m_Mashers.push_back(masher2);

    m_Mashers.push_back(new Masher(2750, 1160));

    Masher* masher3 = new Masher(2670, 1160);
    masher3->SetJumpDelay(45);  // Offset by ~0.75 seconds so they're not synchronized
    m_Mashers.push_back(masher3);

    // Start all masher animations
    for (auto* masher : m_Mashers)
    {
        masher->StartAnimation();
    }

    // Create bridge (decorative element)
    m_Bridge = new Bridge(1056, 1132);
    m_Bridge->StartAnimation();

    // Create bridge (decorative element)
    m_Bridge2 = new Bridge(2592, 1019);
    m_Bridge2->StartAnimation();

    // Create the final ring at the end of the level (goal ring)
    m_FinalRing = new FinalRing(9860, 1350);
    m_FinalRing->StartAnimation();

    // Set up callback for when the final ring collection animation finishes
    m_FinalRing->SetOnGameEnd([this]() {
        StartEndingSequence();
    });

    // Load ending screen films
    m_EndingSonicFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("ending.sonic")
    );
    m_EndingLogoFilm = const_cast<anim::AnimationFilm*>(
        anim::AnimationFilmHolder::Get().GetFilm("ending.logo")
    );

    // Initialize game stats for this level
    GameStats::Get().Reset();
    GameStats::Get().StartTimer();

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
    sound::SetTrackVolume(m_BackgroundMusic, 0.3f);  // Lower volume so SFX can be heard

    // Configure game loop
    m_Game.SetRenderLoop([this]() { OnRender(); });
    m_Game.SetInputLoop([this]() { OnInput(); });
    m_Game.SetAnimationLoop([]() {
        core::SystemClock::Get().SetCurrTime();
        TimeStamp currTime = core::SystemClock::Get().GetCurrTime();
        anim::AnimatorManager::Get().Progress(currTime);
        GameStats::Get().UpdateTimer(currTime);
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
    for (auto* ring : m_ScatteredRings)
    {
        checker.Cancel(m_Sonic, ring);
    }
    for (auto* masher : m_Mashers)
    {
        checker.Cancel(m_Sonic, masher);
    }
    if (m_Checkpoint)
    {
        checker.Cancel(m_Sonic, m_Checkpoint);
    }
    if (m_FinalRing)
    {
        checker.Cancel(m_Sonic, m_FinalRing);
    }

    // Destroy sprites using the engine's destruction system
    for (auto* ring : m_Rings)
    {
        ring->Destroy();
    }
    m_Rings.clear();

    for (auto* ring : m_ScatteredRings)
    {
        ring->Destroy();
    }
    m_ScatteredRings.clear();

    for (auto* flower : m_Flowers)
    {
        flower->Destroy();
    }
    m_Flowers.clear();

    for (auto* masher : m_Mashers)
    {
        masher->Destroy();
    }
    m_Mashers.clear();

    if (m_Bridge)
    {
        m_Bridge->Destroy();
        m_Bridge = nullptr;
    }

      if (m_Bridge2)
    {
        m_Bridge2->Destroy();
        m_Bridge2 = nullptr;
    }

    if (m_Checkpoint)
    {
        m_Checkpoint->Destroy();
        m_Checkpoint = nullptr;
    }

    if (m_FinalRing)
    {
        m_FinalRing->Destroy();
        m_FinalRing = nullptr;
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

        // Background world position: shifted up 700px from original 1.5 rows position
        constexpr int BG_WORLD_Y = -366;      // 384 - 750 = -366
        constexpr int BG_WORLD_HEIGHT = 1024; // 4 rows × 256 pixels

        // Scale background to fit the 4-row height while maintaining aspect ratio
        float scale = static_cast<float>(BG_WORLD_HEIGHT) / bgH;
        int scaledW = static_cast<int>(bgW * scale);
        int scaledH = BG_WORLD_HEIGHT;

        // Calculate screen Y position based on camera with vertical parallax (20% = 1/5th)
        // Background stays mostly fixed, moving only 20% of camera's vertical movement
        int parallaxY = (m_CameraY * 20) / 100;
        int bgScreenY = BG_WORLD_Y - parallaxY;

        // Only render if background is visible on screen
        if (bgScreenY < vpH && bgScreenY + scaledH > 0)
        {
            // Parallax scroll at 30% of camera speed horizontally
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

    // Render bridge
    if (m_Bridge && m_Bridge->IsVisible())
    {
        m_Bridge->Display(screen, viewArea, m_Clipper);
    }

    // Render bridge2
    if (m_Bridge2 && m_Bridge2->IsVisible())
    {
        m_Bridge2->Display(screen, viewArea, m_Clipper);
    }

    // Render rings (visibility is managed by the ring's animation callbacks)
    for (auto* ring : m_Rings)
    {
        if (ring->IsVisible())
        {
            ring->Display(screen, viewArea, m_Clipper);
        }
    }

    // Render scattered rings
    for (auto* ring : m_ScatteredRings)
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

    // Render final ring (goal)
    if (m_FinalRing && m_FinalRing->IsVisible())
    {
        m_FinalRing->Display(screen, viewArea, m_Clipper);
    }

    // Render enemies (Mashers)
    for (auto* masher : m_Mashers)
    {
        if (masher->IsVisible() && masher->IsAlive())
        {
            masher->Display(screen, viewArea, m_Clipper);
        }
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

    // Render HUD overlay (always on top, fixed screen position)
    if (m_EndingState == EndingState::NONE || m_EndingState == EndingState::WAITING)
    {
        m_HUD.Render(screen);
    }

    // Render ending sequence overlay
    if (m_EndingState != EndingState::NONE && m_FadeAlpha > 0.0f)
    {
        // Draw fade overlay using direct pixel manipulation
        if (gfx::BitmapLock(screen))
        {
            uint8_t* base = gfx::BitmapGetMemory(screen);
            int pitch = gfx::BitmapGetLineOffset(screen);
            uint8_t fadeAmount = static_cast<uint8_t>(m_FadeAlpha * 255.0f);

            // Get color component extraction info
            unsigned redShift = gfx::GetRedShiftRGBA();
            unsigned greenShift = gfx::GetGreenShiftRGBA();
            unsigned blueShift = gfx::GetBlueShiftRGBA();

            for (int y = 0; y < vpH; ++y)
            {
                gfx::Color* row = reinterpret_cast<gfx::Color*>(base + y * pitch);
                for (int x = 0; x < vpW; ++x)
                {
                    // Extract color components
                    uint8_t r = static_cast<uint8_t>((row[x] >> redShift) & 0xFF);
                    uint8_t g = static_cast<uint8_t>((row[x] >> greenShift) & 0xFF);
                    uint8_t b = static_cast<uint8_t>((row[x] >> blueShift) & 0xFF);

                    // Blend towards black (0, 0, 0)
                    r = static_cast<uint8_t>((r * (255 - fadeAmount)) / 255);
                    g = static_cast<uint8_t>((g * (255 - fadeAmount)) / 255);
                    b = static_cast<uint8_t>((b * (255 - fadeAmount)) / 255);
                    row[x] = gfx::MakeColor(r, g, b, 255);
                }
            }
            gfx::BitmapUnlock(screen);
        }

        // Render end screen sprites when fully faded
        if (m_EndingState == EndingState::SHOWING_END && m_EndingSonicFilm && m_EndingLogoFilm)
        {
            // Get sprite dimensions
            Rect sonicFrame = m_EndingSonicFilm->GetFrameBox(0);
            Rect logoFrame = m_EndingLogoFilm->GetFrameBox(0);

            // Center the combined image on screen
            // Sonic on left, logo on right with some spacing
            int totalWidth = sonicFrame.w + 20 + logoFrame.w;  // 20px spacing
            int startX = (vpW - totalWidth) / 2;
            int sonicY = (vpH - sonicFrame.h) / 2;
            int logoY = (vpH - logoFrame.h) / 2 - 30;  // 50px higher than center

            // Draw Sonic sprite
            gfx::BitmapBlit(
                m_EndingSonicFilm->GetBitmap(),
                sonicFrame,
                screen,
                { startX, sonicY }
            );

            // Draw logo sprite
            gfx::BitmapBlit(
                m_EndingLogoFilm->GetBitmap(),
                logoFrame,
                screen,
                { startX + sonicFrame.w + 20, logoY }
            );
        }
    }

    // Render pause menu overlay if game is paused
    if (m_Game.IsPaused())
    {
        RenderPauseMenu(screen, vpW, vpH);
    }

    gfx::Flush();

    // Cap at ~60 FPS
    std::this_thread::sleep_for(std::chrono::milliseconds(12));
}

void GameScene::OnInput()
{
    core::Input::UpdateInputEvents();

    // Skip gameplay input when paused (menu input is handled via key events)
    if (m_Game.IsPaused())
    {
        return;
    }

    // Handle ending sequence updates
    if (m_EndingState != EndingState::NONE)
    {
        UpdateEndingSequence();

        // Stop gameplay updates once the end screen is showing
        if (m_EndingState == EndingState::SHOWING_END || m_EndingState == EndingState::DONE)
        {
            return;
        }
        // During WAITING and FADING, Sonic can still move (fall through to normal updates)
    }

    // Update enemies
    for (auto* masher : m_Mashers)
    {
        if (masher->IsAlive())
        {
            masher->Update();
        }
    }

    // Update scattered rings (physics and cleanup)
    UpdateScatteredRings();

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

    // Handle pause menu input when paused
    if (m_Game.IsPaused())
    {
        HandlePauseMenuInput(key);
        return;
    }

    if (key == io::Key::G)
    {
        m_ShowGrid = !m_ShowGrid;
    }
    else if (key == io::Key::Escape)
    {
        // Pause the game and show pause menu
        m_PauseSelection = PauseMenuOption::CONTINUE;
        m_Game.Pause(core::SystemClock::Get().GetCurrTime());
    }
}

void GameScene::HandlePauseMenuInput(io::Key key)
{
    if (key == io::Key::W || key == io::Key::Up)
    {
        // Move selection up
        int sel = static_cast<int>(m_PauseSelection);
        sel = (sel - 1 + PAUSE_MENU_OPTIONS) % PAUSE_MENU_OPTIONS;
        m_PauseSelection = static_cast<PauseMenuOption>(sel);
    }
    else if (key == io::Key::S || key == io::Key::Down)
    {
        // Move selection down
        int sel = static_cast<int>(m_PauseSelection);
        sel = (sel + 1) % PAUSE_MENU_OPTIONS;
        m_PauseSelection = static_cast<PauseMenuOption>(sel);
    }
    else if (key == io::Key::Enter || key == io::Key::Space)
    {
        // Select current option
        if (m_PauseSelection == PauseMenuOption::CONTINUE)
        {
            m_Game.Resume();
        }
        else if (m_PauseSelection == PauseMenuOption::RESTART)
        {
            m_ShouldExit = true;
            SceneManager::Get().ChangeScene(SceneType::GAME);  // Restart the game
        }
        else if (m_PauseSelection == PauseMenuOption::EXIT)
        {
            m_ShouldExit = true;
            SceneManager::Get().ChangeScene(SceneType::MENU);
        }
    }
    else if (key == io::Key::Escape)
    {
        // ESC while paused resumes the game
        m_Game.Resume();
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

    // Register Sonic vs FinalRing (goal)
    if (m_FinalRing)
    {
        checker.Register(m_Sonic, m_FinalRing,
            [this](scene::Sprite* sonic, scene::Sprite* finalRingSprite) {
                FinalRing* finalRing = static_cast<FinalRing*>(finalRingSprite);
                if (!finalRing->IsCollected())
                {
                    // Stop background music to let stage clear theme play
                    if (m_BackgroundMusic)
                    {
                        sound::StopTrack(m_BackgroundMusic, 500);  // 500ms fade out
                    }
                    finalRing->OnCollected();
                }
            }
        );
    }

    // Register Sonic vs Mashers (enemies)
    for (auto* masher : m_Mashers)
    {
        checker.Register(m_Sonic, masher,
            [](scene::Sprite* sonicSprite, scene::Sprite* masherSprite) {
                Sonic* sonic = static_cast<Sonic*>(sonicSprite);
                Masher* masher = static_cast<Masher*>(masherSprite);
                if (!masher->IsAlive())
                    return;

                // When invincible, Sonic can't interact with enemies at all
                if (sonic->IsInvincible())
                    return;

                // If Sonic is in ball state (spinning/jumping), he kills the enemy
                if (sonic->IsInBallState())
                {
                    masher->Kill();
                    sonic->BounceOffEnemy();
                }
                // Otherwise, Sonic takes damage
                else
                {
                    sonic->OnHit();
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

void GameScene::LoadRings()
{
    std::ifstream file(std::string(ASSETS) + "/Data/rings.json");
    if (!file.is_open())
    {
        return;
    }

    nlohmann::json data;
    file >> data;
    file.close();

    for (const auto& pos : data["rings"])
    {
        int x = pos[0];
        int y = pos[1];
        m_Rings.push_back(new Ring(x, y));
    }
}

void GameScene::RenderPauseMenu(gfx::Bitmap screen, int vpW, int vpH)
{
    // Draw semi-transparent dark overlay
    if (gfx::BitmapLock(screen))
    {
        uint8_t* base = gfx::BitmapGetMemory(screen);
        int pitch = gfx::BitmapGetLineOffset(screen);
        constexpr uint8_t darkenAmount = 128;  // 50% darkening

        unsigned redShift = gfx::GetRedShiftRGBA();
        unsigned greenShift = gfx::GetGreenShiftRGBA();
        unsigned blueShift = gfx::GetBlueShiftRGBA();

        for (int y = 0; y < vpH; ++y)
        {
            gfx::Color* row = reinterpret_cast<gfx::Color*>(base + y * pitch);
            for (int x = 0; x < vpW; ++x)
            {
                uint8_t r = static_cast<uint8_t>((row[x] >> redShift) & 0xFF);
                uint8_t g = static_cast<uint8_t>((row[x] >> greenShift) & 0xFF);
                uint8_t b = static_cast<uint8_t>((row[x] >> blueShift) & 0xFF);

                r = static_cast<uint8_t>((r * (255 - darkenAmount)) / 255);
                g = static_cast<uint8_t>((g * (255 - darkenAmount)) / 255);
                b = static_cast<uint8_t>((b * (255 - darkenAmount)) / 255);
                row[x] = gfx::MakeColor(r, g, b, 255);
            }
        }
        gfx::BitmapUnlock(screen);
    }

    // Menu dimensions
    constexpr int BUTTON_WIDTH = 120;
    constexpr int BUTTON_HEIGHT = 25;
    constexpr int BUTTON_SPACING = 10;
    constexpr int MENU_PADDING = 20;

    int menuHeight = 3 * BUTTON_HEIGHT + 2 * BUTTON_SPACING + 2 * MENU_PADDING + 30; // +30 for title
    int menuWidth = BUTTON_WIDTH + 2 * MENU_PADDING;
    int menuX = (vpW - menuWidth) / 2;
    int menuY = (vpH - menuHeight) / 2;

    // Draw menu background
    draw::FilledRect(screen, menuX, menuY, menuWidth, menuHeight, menu::COLOR_BUTTON_FACE);
    draw::RectBorder(screen, menuX, menuY, menuWidth, menuHeight, menu::COLOR_BUTTON_DARK, 3);

    // Draw "PAUSED" title
    const char* title = "PAUSED";
    int titleX = menuX + (menuWidth - 6 * 6 * 2) / 2;  // 6 chars * 6px * scale 2
    int titleY = menuY + MENU_PADDING;
    draw::Text(screen, titleX, titleY, title, menu::COLOR_TEXT, 2);

    // Button positions
    int buttonX = menuX + MENU_PADDING;
    int button1Y = menuY + MENU_PADDING + 30;
    int button2Y = button1Y + BUTTON_HEIGHT + BUTTON_SPACING;
    int button3Y = button2Y + BUTTON_HEIGHT + BUTTON_SPACING;

    // Draw Continue button
    bool continueSelected = (m_PauseSelection == PauseMenuOption::CONTINUE);
    draw::StoneButton(screen, buttonX, button1Y, BUTTON_WIDTH, BUTTON_HEIGHT, continueSelected);
    int textX = buttonX + (BUTTON_WIDTH - 8 * 6) / 2;  // "CONTINUE" = 8 chars
    draw::Text(screen, textX, button1Y + 8, "CONTINUE", menu::COLOR_TEXT, 1);

    // Draw Restart button
    bool restartSelected = (m_PauseSelection == PauseMenuOption::RESTART);
    draw::StoneButton(screen, buttonX, button2Y, BUTTON_WIDTH, BUTTON_HEIGHT, restartSelected);
    textX = buttonX + (BUTTON_WIDTH - 7 * 6) / 2;  // "RESTART" = 7 chars
    draw::Text(screen, textX, button2Y + 8, "RESTART", menu::COLOR_TEXT, 1);

    // Draw Exit button
    bool exitSelected = (m_PauseSelection == PauseMenuOption::EXIT);
    draw::StoneButton(screen, buttonX, button3Y, BUTTON_WIDTH, BUTTON_HEIGHT, exitSelected);
    textX = buttonX + (BUTTON_WIDTH - 4 * 6) / 2;  // "EXIT" = 4 chars
    draw::Text(screen, textX, button3Y + 8, "EXIT", menu::COLOR_TEXT, 1);

    // Draw selection arrow
    int arrowY = button1Y;
    if (m_PauseSelection == PauseMenuOption::RESTART) arrowY = button2Y;
    else if (m_PauseSelection == PauseMenuOption::EXIT) arrowY = button3Y;
    draw::Arrow(screen, buttonX - 15, arrowY + BUTTON_HEIGHT / 2 - 4, menu::COLOR_ARROW);
}

void GameScene::StartEndingSequence()
{
    if (m_EndingState != EndingState::NONE)
        return;  // Already in ending sequence

    m_EndingState = EndingState::WAITING;
    m_EndingStartTime = core::SystemClock::Get().GetCurrTime();
    m_FadeAlpha = 0.0f;
}

void GameScene::UpdateEndingSequence()
{
    TimeStamp currentTime = core::SystemClock::Get().GetCurrTime();
    TimeStamp elapsed = currentTime - m_EndingStartTime;

    switch (m_EndingState)
    {
        case EndingState::WAITING:
            // Wait 1 second before starting fade
            if (elapsed >= ENDING_WAIT_MS)
            {
                m_EndingState = EndingState::FADING;
                m_EndingStartTime = currentTime;
            }
            break;

        case EndingState::FADING:
            // Gradually increase fade alpha over ENDING_FADE_MS
            m_FadeAlpha = static_cast<float>(elapsed) / static_cast<float>(ENDING_FADE_MS);
            if (m_FadeAlpha >= 1.0f)
            {
                m_FadeAlpha = 1.0f;
                m_EndingState = EndingState::SHOWING_END;
                m_EndingStartTime = currentTime;
            }
            break;

        case EndingState::SHOWING_END:
            // Show end screen for 5 seconds
            if (elapsed >= ENDING_DISPLAY_MS)
            {
                m_EndingState = EndingState::DONE;
                m_ShouldExit = true;
                SceneManager::Get().ChangeScene(SceneType::MENU);
            }
            break;

        case EndingState::DONE:
        case EndingState::NONE:
            // Nothing to do
            break;
    }
}

void GameScene::SpawnScatteredRings(int x, int y, int count)
{
    // Play ring drop sound
    if (count > 0 && s_RingDropSound)
    {
        sound::PlaySFX(s_RingDropSound);
    }

    // Cap at 32 rings for performance
    count = (count > 32) ? 32 : count;

    // Random number generator for natural scatter pattern
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> speedDist(2.5f, 5.0f);     // Random speed
    std::uniform_real_distribution<float> angleDist(-0.6f, 0.6f);    // Random angle offset (~35 degrees each way)

    for (int i = 0; i < count; ++i)
    {
        // Random speed and angle for natural scatter
        float speed = speedDist(gen);
        float angleOffset = angleDist(gen);
        float angle = static_cast<float>(-M_PI / 2.0) + angleOffset;  // Centered on upward

        // Calculate velocity with upward bias
        float vx = speed * std::cos(angle);
        float vy = speed * std::sin(angle) - 1.5f;  // Extra upward boost

        // Create the scattered ring
        auto* ring = new ScatteredRing(x, y, vx, vy);
        ring->StartAnimation();
        m_ScatteredRings.push_back(ring);

        // Register collision with Sonic
        physics::CollisionChecker::Get().Register(m_Sonic, ring,
            [](scene::Sprite* sonicSprite, scene::Sprite* ringSprite) {
                ScatteredRing* ring = static_cast<ScatteredRing*>(ringSprite);
                if (ring->IsCollectable() && !ring->IsCollected())
                {
                    ring->OnCollected();
                }
            }
        );
    }
}

void GameScene::UpdateScatteredRings()
{
    // Update all scattered rings (physics)
    for (auto* ring : m_ScatteredRings)
    {
        ring->Update();
    }

    // Remove expired or collected rings
    auto it = m_ScatteredRings.begin();
    while (it != m_ScatteredRings.end())
    {
        if ((*it)->IsExpired() || (*it)->IsCollectionFinished())
        {
            physics::CollisionChecker::Get().Cancel(m_Sonic, *it);
            (*it)->Destroy();
            it = m_ScatteredRings.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
