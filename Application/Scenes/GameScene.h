#pragma once

#include "Core/Context.h"
#include "Core/EventRegistry.h"
#include "Rendering/Bitmap.h"
#include "Rendering/Clipper.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"

#include "Sprites/Ring.h"
#include "Sprites/Checkpoint.h"
#include "Sprites/BasicSprite.h"
#include "Sprites/Flower.h"
#include "Sprites/Masher.h"
#include "Physics/CollisionChecker.h"
#include "Sound/Sound.h"
#include "Game/HUD.h"
#include "Animations/TunnelPath.h"

#include <vector>

class GameScene : public core::Context
{
public:
    void Initialize() override;
    void Load() override;
    void Clear() override;

private:
    void OnRender();
    void OnInput();
    void HandleKeyEvent(io::Key key);
    void HandleCloseEvent();
    void ClampCamera();
    void RegisterCollisions();
    void OnCollisionCheckLoop();
    void LoadFlowers();

private:
    // Event handles
    core::EventHandle m_CloseHandle;
    core::EventHandle m_KeyHandle;

    // Resources
    gfx::BitmapLoader m_Loader;
    gfx::Bitmap m_ParallaxBackground = nullptr;
    scene::TileLayer m_TileLayer;
    scene::GridMap m_Grid;

    // Camera state
    int m_CameraX = 0;
    int m_CameraY = 0;

    // Vertical camera smoothing (follows ground level, not Sonic's Y directly)
    float m_CameraTargetY = 0.0f;    // Target Y based on last ground contact
    float m_CameraSmoothY = 0.0f;    // Current smoothed Y position
    bool m_CameraInitialized = false; // First frame initialization flag

    // Display state
    bool m_ShowGrid = false;
    bool m_ShouldExit = false;

    // Sprites
    std::vector<Ring*> m_Rings;
    std::vector<Flower*> m_Flowers;
    std::vector<Masher*> m_Mashers;
    Checkpoint* m_Checkpoint = nullptr;
    Sonic* m_Sonic = nullptr;
    gfx::Clipper m_Clipper;

    // Audio
    sound::Track m_BackgroundMusic = nullptr;

    // HUD
    HUD m_HUD;

    // Tunnel paths
    std::vector<anim::TunnelPath> m_TunnelPaths;

    // Level constants
    static constexpr int LEVEL_WIDTH = 10240;
    static constexpr int LEVEL_HEIGHT = 1536;
    static constexpr int SCROLL_SPEED = 4;
    static constexpr int GRID_Y_OFFSET = 0;  // Full-height 1x1 grid covers entire level
};
