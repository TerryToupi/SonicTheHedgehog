#include "Scenes/CreditsScene.h"
#include "Scenes/SceneManager.h"
#include "Rendering/Renderer.h"
#include "Core/Input.h"
#include "Utilities/DrawHelpers.h"

#include <thread>
#include <chrono>

void CreditsScene::Initialize()
{
    m_FlashCounter = 0;
    m_ShouldExit = false;
}

void CreditsScene::Load()
{
    m_CloseHandle = core::EventRegistry::Subscribe(EventType::CLOSE_EVENT,
        [this]() { HandleCloseEvent(); });

    m_KeyHandle = core::EventRegistry::Subscribe(EventType::KEY_EVENT,
        [this](io::Key key) { HandleKeyEvent(key); });

    m_MouseButtonHandle = core::EventRegistry::Subscribe(EventType::MOUSE_BUTTON_EVENT,
        [this](io::Button btn) { HandleMouseButton(btn); });

    m_Game.SetRenderLoop([this]() { OnRender(); });
    m_Game.SetInputLoop([this]() { OnInput(); });
    m_Game.SetFinishingFunc([this]() { return !m_ShouldExit; });
}

void CreditsScene::Clear()
{
    // Explicitly reset event handles to unsubscribe before destruction
    m_CloseHandle = core::EventHandle();
    m_KeyHandle = core::EventHandle();
    m_MouseButtonHandle = core::EventHandle();
}

void CreditsScene::OnRender()
{
    int vpW = SceneManager::Get().GetViewportWidth();
    int vpH = SceneManager::Get().GetViewportHeight();
    gfx::Bitmap screen = gfx::GetScreenBuffer();

    // Dark blue background
    gfx::BitmapClear(screen, gfx::MakeColor(0, 0, 64, 255));

    // Content area
    int contentX = 20;
    int contentY = 30;
    int contentW = vpW - 40;
    int contentH = vpH - 60;
    draw::FilledRect(screen, contentX, contentY, contentW, contentH,
                     gfx::MakeColor(0, 0, 96, 255));
    draw::RectBorder(screen, contentX, contentY, contentW, contentH,
                     gfx::MakeColor(100, 100, 200, 255), 2);

    // Credits text
    gfx::Color titleColor = gfx::MakeColor(255, 215, 0, 255);   // Gold
    gfx::Color textColor = gfx::MakeColor(255, 255, 255, 255);  // White
    gfx::Color shadowColor = gfx::MakeColor(0, 0, 40, 255);

    // Title
    draw::Text(screen, 121, 45, "CREDITS", shadowColor);
    draw::Text(screen, 120, 44, "CREDITS", titleColor);

    // Lefteris Toupis, CSDP1457
    draw::Text(screen, 71, 80, "LEFTERIS TOUPIS", shadowColor);
    draw::Text(screen, 70, 79, "LEFTERIS TOUPIS", textColor);
    draw::Text(screen, 111, 95, "CSDP1457", shadowColor);
    draw::Text(screen, 110, 94, "CSDP1457", textColor);

    // Mike Giannakopoulos, CSDP1464
    draw::Text(screen, 41, 125, "MIKE GIANNAKOPOULOS", shadowColor);
    draw::Text(screen, 40, 124, "MIKE GIANNAKOPOULOS", textColor);
    draw::Text(screen, 111, 140, "CSDP1464", shadowColor);
    draw::Text(screen, 110, 139, "CSDP1464", textColor);

    // Flashing hint
    m_FlashCounter = (m_FlashCounter + 1) % 60;
    if (m_FlashCounter < 30)
    {
        gfx::Color hintColor = gfx::MakeColor(180, 180, 255, 255);
        draw::Text(screen, 80, 185, "PRESS ESC", hintColor);
    }

    gfx::Flush();

    // Cap at ~60 FPS
    std::this_thread::sleep_for(std::chrono::milliseconds(12));
}

void CreditsScene::OnInput()
{
    core::Input::UpdateInputEvents();
}

void CreditsScene::HandleKeyEvent(io::Key key)
{
    if (m_ShouldExit) return;  // Already transitioning

    // Only Escape returns to menu (avoids rapid key press issues)
    if (key == io::Key::Escape)
    {
        m_ShouldExit = true;
        SceneManager::Get().ChangeScene(SceneType::MENU);
    }
}

void CreditsScene::HandleMouseButton(io::Button button)
{
    if (m_ShouldExit) return;  // Already transitioning
    if (button == io::Button::Left)
    {
        m_ShouldExit = true;
        SceneManager::Get().ChangeScene(SceneType::MENU);
    }
}

void CreditsScene::HandleCloseEvent()
{
    if (m_ShouldExit) return;  // Already transitioning
    m_ShouldExit = true;
    SceneManager::Get().ChangeScene(SceneType::EXIT);
}
