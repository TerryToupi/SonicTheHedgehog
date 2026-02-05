#pragma once

#include "Core/Context.h"
#include "Core/EventRegistry.h"
#include "Rendering/Bitmap.h"
#include "Rendering/Clipper.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"

#include "Sprites/Ring.h"
#include "Sprites/ScatteredRing.h"
#include "Sprites/FinalRing.h"
#include "Sprites/Checkpoint.h"
#include "Sprites/BasicSprite.h"
#include "Sprites/Flower.h"
#include "Sprites/Masher.h"
#include "Sprites/Crabmeat.h"
#include "Sprites/CrabBullet.h"
#include "Sprites/Bridge.h"
#include "Physics/CollisionChecker.h"
#include "Sound/Sound.h"
#include "Game/HUD.h"
#include "Animations/TunnelPath.h"
#include "Animations/AnimationFilm.h"

#include <vector>

class GameScene : public core::Context
{
public:
    void Initialize() override;
    void Load() override;
    void Clear() override;

    // Called by FinalRing when collected
    void StartEndingSequence();

private:
    // Pause menu options
    enum class PauseMenuOption
    {
        CONTINUE = 0,
        RESTART = 1,
        EXIT = 2
    };
    static constexpr int PAUSE_MENU_OPTIONS = 3;

    // Ending sequence states
    enum class EndingState
    {
        NONE,           // Normal gameplay
        WAITING,        // 1 second wait after collection
        FADING,         // Screen darkening
        SHOWING_END,    // Displaying end logo
        DONE            // Transitioning to menu
    };

    // Death/respawn sequence states
    enum class DeathState
    {
        NONE,           // Normal gameplay
        FADING_OUT,     // Screen darkening after death
        FADING_IN       // Screen brightening at checkpoint
    };

    void OnRender();
    void OnInput();
    void UpdateEndingSequence();
    void HandleKeyEvent(io::Key key);
    void HandleCloseEvent();
    void RenderPauseMenu(gfx::Bitmap screen, int vpW, int vpH);
    void HandlePauseMenuInput(io::Key key);
    void ClampCamera();
    void RegisterCollisions();
    void OnCollisionCheckLoop();
    void LoadFlowers();
    void LoadRings();
    void SpawnScatteredRings(int x, int y, int count);
    void UpdateScatteredRings();

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
    std::vector<ScatteredRing*> m_ScatteredRings;
    std::vector<Flower*> m_Flowers;
    std::vector<Masher*> m_Mashers;
    std::vector<Crabmeat*> m_Crabmeats;
    Bridge* m_Bridge = nullptr;
    Bridge* m_Bridge2 = nullptr;
    std::vector<Checkpoint*> m_Checkpoints;
    FinalRing* m_FinalRing = nullptr;
    Sonic* m_Sonic = nullptr;
    gfx::Clipper m_Clipper;

    // Audio
    sound::Track m_BackgroundMusic = nullptr;
    static sound::SFX s_RingDropSound;

    // HUD
    HUD m_HUD;

    // Tunnel paths
    std::vector<anim::TunnelPath> m_TunnelPaths;

    // Level constants
    static constexpr int LEVEL_WIDTH = 10240;
    static constexpr int LEVEL_HEIGHT = 1536;
    static constexpr int SCROLL_SPEED = 4;
    static constexpr int GRID_Y_OFFSET = 0;  // Full-height 1x1 grid covers entire level

    // Respawn system
    Point m_RespawnPosition = {50, 1100};  // Default spawn position
    void OnSonicDeath();
    void RespawnSonic();
    void UpdateDeathSequence();

    // Death sequence state
    DeathState m_DeathState = DeathState::NONE;
    TimeStamp m_DeathStartTime = 0;
    float m_DeathFadeAlpha = 0.0f;

    // Death timing constants (in milliseconds)
    static constexpr int DEATH_FADE_OUT_MS = 800;   // Fade to black
    static constexpr int DEATH_FADE_IN_MS = 800;    // Fade back in

    // Ending sequence
    EndingState m_EndingState = EndingState::NONE;
    TimeStamp m_EndingStartTime = 0;
    float m_FadeAlpha = 0.0f;
    anim::AnimationFilm* m_EndingSonicFilm = nullptr;
    anim::AnimationFilm* m_EndingLogoFilm = nullptr;

    // Ending timing constants (in milliseconds)
    static constexpr int ENDING_WAIT_MS = 1000;       // 1 second wait before fade
    static constexpr int ENDING_FADE_MS = 1500;       // 1.5 seconds to fade to dark
    static constexpr int ENDING_DISPLAY_MS = 5000;    // 5 seconds showing end screen

    // Pause menu state
    PauseMenuOption m_PauseSelection = PauseMenuOption::CONTINUE;
};
