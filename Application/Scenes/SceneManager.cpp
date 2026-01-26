#include "Scenes/SceneManager.h"
#include "Scenes/MenuScene.h"
#include "Scenes/GameScene.h"
#include "Scenes/CreditsScene.h"
#include "Rendering/Renderer.h"
#include "Sound/Sound.h"
#include "Core/Input.h"

SceneManager& SceneManager::Get()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::Initialize(const char* windowTitle, int windowWidth, int windowHeight,
                               int viewportWidth, int viewportHeight)
{
    if (m_Initialized) return;

    m_ViewportWidth = viewportWidth;
    m_ViewportHeight = viewportHeight;
    m_WindowWidth = windowWidth;
    m_WindowHeight = windowHeight;

    gfx::Open(windowTitle, windowWidth, windowHeight);
    gfx::SetScreenBuffer(viewportWidth, viewportHeight);
    sound::Open();

    m_Initialized = true;
}

void SceneManager::Shutdown()
{
    if (!m_Initialized) return;

    sound::Close();
    gfx::Close();

    m_Initialized = false;
}

void SceneManager::ChangeScene(SceneType nextScene)
{
    m_NextScene = nextScene;
}

SceneType SceneManager::GetNextScene() const
{
    return m_NextScene;
}

int SceneManager::GetViewportWidth() const
{
    return m_ViewportWidth;
}

int SceneManager::GetViewportHeight() const
{
    return m_ViewportHeight;
}

int SceneManager::GetWindowWidth() const
{
    return m_WindowWidth;
}

int SceneManager::GetWindowHeight() const
{
    return m_WindowHeight;
}

void SceneManager::Run()
{
    while (m_NextScene != SceneType::EXIT && m_NextScene != SceneType::NONE)
    {
        SceneType currentSceneType = m_NextScene;
        m_NextScene = SceneType::NONE;  // Reset so scene must explicitly set next

        // Flush any pending input events before starting new scene
        core::Input::FlushEvents();

        auto scene = CreateScene(currentSceneType);
        if (scene)
        {
            scene->Start();  // Runs Initialize -> Load -> Run -> Clear
        }

        // If scene didn't set a next scene, default back to MENU
        if (m_NextScene == SceneType::NONE)
        {
            m_NextScene = SceneType::MENU;
        }
    }
}

std::unique_ptr<core::Context> SceneManager::CreateScene(SceneType type)
{
    switch (type)
    {
    case SceneType::MENU:
        return std::make_unique<MenuScene>();
    case SceneType::GAME:
        return std::make_unique<GameScene>();
    case SceneType::CREDITS:
        return std::make_unique<CreditsScene>();
    default:
        return nullptr;
    }
}
