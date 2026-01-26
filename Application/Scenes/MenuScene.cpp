#include "Scenes/MenuScene.h"
#include "Scenes/SceneManager.h"
#include "Rendering/Renderer.h"
#include "Core/Input.h"
#include "Utilities/DrawHelpers.h"
#include "Utilities/MenuConstants.h"

#include <string>
#include <thread>
#include <chrono>

void MenuScene::Initialize()
{
    // Initialize button positions
    const int VIEWPORT_WIDTH = SceneManager::Get().GetViewportWidth();
    const int BUTTON_WIDTH = 80;
    const int BUTTON_HEIGHT = 20;
    const int BUTTON_X = (VIEWPORT_WIDTH - BUTTON_WIDTH) / 2;

    m_Buttons[0] = { BUTTON_X, 156, BUTTON_WIDTH, BUTTON_HEIGHT };  // Play
    m_Buttons[1] = { BUTTON_X, 178, BUTTON_WIDTH, BUTTON_HEIGHT };  // Credits
    m_Buttons[2] = { BUTTON_X, 200, BUTTON_WIDTH, BUTTON_HEIGHT };  // Exit

    m_SelectedButton = 0;
    m_ShouldExit = false;
    m_StartupFrames = 0;
}

void MenuScene::Load()
{
    // Load resources (using member loader so bitmaps persist)
    m_MenuBackground = m_Loader.Load(std::string(ASSETS) + "/Textures/sonic_menu.png");

    // m_MenuBackground may be null if the file doesn't exist

    // Subscribe to events
    m_CloseHandle = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT,
        [this]() { HandleCloseEvent(); });

    m_KeyHandle = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
        [this](io::Key key) { HandleKeyEvent(key); });

    m_MouseMotionHandle = core::EventRegistry::Subscribe(EventType::MOUSE_MORION_EVENT,
        [this](int x, int y) { HandleMouseMotion(x, y); });

    m_MouseButtonHandle = core::EventRegistry::Subscribe(EventType::MOUSE_BUTTON_EVENT,
        [this](io::Button btn) { HandleMouseButton(btn); });

    // Configure game loop
    m_Game.SetRenderLoop([this]() { OnRender(); });
    m_Game.SetInputLoop([this]() { OnInput(); });
    m_Game.SetFinishingFunc([this]() { return !m_ShouldExit; });
}

void MenuScene::Clear()
{
    // Explicitly reset event handles to unsubscribe before destruction
    m_CloseHandle = core::EventHandle();
    m_KeyHandle = core::EventHandle();
    m_MouseMotionHandle = core::EventHandle();
    m_MouseButtonHandle = core::EventHandle();
}

void MenuScene::OnRender()
{
    gfx::Bitmap screen = gfx::GetScreenBuffer();
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();

    gfx::BitmapClear(screen, gfx::MakeColor(0, 0, 0, 255));

    // Draw background scaled to viewport
    if (m_MenuBackground)
    {
        int bgW = gfx::BitmapGetWidth(m_MenuBackground);
        int bgH = gfx::BitmapGetHeight(m_MenuBackground);
        gfx::BitmapBlitScaled(m_MenuBackground, {0, 0, bgW, bgH}, screen, {0, 0, vpW, vpH});
    }

    // Draw buttons
    const char* labels[] = { "PLAY", "CREDITS", "EXIT" };
    const int labelLengths[] = { 4, 7, 4 };  // Character counts
    const int ARROW_OFFSET = 12;

    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        bool selected = (i == m_SelectedButton);
        draw::StoneButton(screen, m_Buttons[i].x, m_Buttons[i].y,
                          m_Buttons[i].width, m_Buttons[i].height, selected);

        // Calculate text position (centered in button)
        int textLen = labelLengths[i];
        int textW = textLen * 6 - 1;
        int textX = m_Buttons[i].x + (m_Buttons[i].width - textW) / 2;
        int textY = m_Buttons[i].y + (m_Buttons[i].height - 7) / 2;

        // Draw text shadow then text
        draw::Text(screen, textX + 1, textY + 1, labels[i], menu::COLOR_TEXT_SHADOW);
        draw::Text(screen, textX, textY, labels[i], menu::COLOR_TEXT);

        // Selection arrow
        if (selected)
        {
            int arrowX = m_Buttons[i].x - ARROW_OFFSET - 2;
            int arrowY = m_Buttons[i].y + (m_Buttons[i].height - 10) / 2;
            draw::Arrow(screen, arrowX, arrowY, menu::COLOR_ARROW);
        }
    }

    gfx::Flush();

    // Cap at ~60 FPS
    std::this_thread::sleep_for(std::chrono::milliseconds(12));
}

void MenuScene::OnInput()
{
    if (m_StartupFrames < 5)
        ++m_StartupFrames;
    core::Input::UpdateInputEvents();
}

void MenuScene::HandleKeyEvent(io::Key key)
{
    if (m_StartupFrames < 3) return;  // Ignore input during startup
    if (m_ShouldExit) return;  // Already transitioning

    if (key == io::Key::W || key == io::Key::Up)
    {
        m_SelectedButton = (m_SelectedButton > 0) ? m_SelectedButton - 1 : BUTTON_COUNT - 1;
    }
    else if (key == io::Key::S || key == io::Key::Down)
    {
        m_SelectedButton = (m_SelectedButton < BUTTON_COUNT - 1) ? m_SelectedButton + 1 : 0;
    }
    else if (key == io::Key::Space || key == io::Key::Enter)
    {
        ActivateSelectedButton();
    }
}

void MenuScene::HandleMouseMotion(int x, int y)
{
    // Convert window coordinates to viewport coordinates
    int windowW = SceneManager::Get().GetWindowWidth();
    int windowH = SceneManager::Get().GetWindowHeight();
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();

    m_MouseX = x * vpW / windowW;
    m_MouseY = y * vpH / windowH;

    // Update selection based on mouse hover
    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        if (menu::IsPointInButton(m_MouseX, m_MouseY, m_Buttons[i]))
        {
            m_SelectedButton = i;
            break;
        }
    }
}

void MenuScene::HandleMouseButton(io::Button button)
{
    if (m_StartupFrames < 3) return;  // Ignore input during startup
    if (m_ShouldExit) return;  // Already transitioning

    if (button == io::Button::Left)
    {
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            if (menu::IsPointInButton(m_MouseX, m_MouseY, m_Buttons[i]))
            {
                m_SelectedButton = i;
                ActivateSelectedButton();
                break;
            }
        }
    }
}

void MenuScene::ActivateSelectedButton()
{
    m_ShouldExit = true;
    switch (m_SelectedButton)
    {
        case 0: SceneManager::Get().ChangeScene(SceneType::GAME); break;
        case 1: SceneManager::Get().ChangeScene(SceneType::CREDITS); break;
        case 2: SceneManager::Get().ChangeScene(SceneType::EXIT); break;
    }
}

void MenuScene::HandleCloseEvent()
{
    if (m_ShouldExit) return;  // Already transitioning
    m_ShouldExit = true;
    SceneManager::Get().ChangeScene(SceneType::EXIT);
}
