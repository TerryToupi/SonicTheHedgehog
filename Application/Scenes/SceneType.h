#pragma once

enum class SceneType
{
    NONE,       // No scene (initial/reset state)
    MENU,       // Main menu
    GAME,       // Gameplay
    CREDITS,    // Credits screen
    EXIT        // Signal to quit application
};
