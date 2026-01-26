#pragma once

#include "Scenes/SceneType.h"
#include "Core/Context.h"

#include <memory>

class SceneManager
{
public:
    static SceneManager& Get();

    // Initialize graphics and sound (call once from main)
    void Initialize(const char* windowTitle, int windowWidth, int windowHeight,
                    int viewportWidth, int viewportHeight);

    // Cleanup graphics and sound (call once before exit)
    void Shutdown();

    // Request a scene change (takes effect after current scene exits)
    void ChangeScene(SceneType nextScene);

    // Get the next requested scene
    SceneType GetNextScene() const;

    // Get viewport dimensions (for scenes to use)
    int GetViewportWidth() const;
    int GetViewportHeight() const;

    // Get window dimensions (for coordinate conversion)
    int GetWindowWidth() const;
    int GetWindowHeight() const;

    // Main application loop - runs scenes until EXIT
    void Run();

private:
    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // Factory method to create scene instances
    std::unique_ptr<core::Context> CreateScene(SceneType type);

private:
    SceneType m_NextScene = SceneType::MENU;
    int m_ViewportWidth = 320;
    int m_ViewportHeight = 224;
    int m_WindowWidth = 1280;
    int m_WindowHeight = 720;
    bool m_Initialized = false;
};
