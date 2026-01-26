#pragma once

#include "Core/Context.h"
#include "Core/EventRegistry.h"
#include "Rendering/Bitmap.h"
#include "Utilities/MenuConstants.h"

class MenuScene : public core::Context
{
public:
    void Initialize() override;
    void Load() override;
    void Clear() override;

private:
    void OnRender();
    void OnInput();
    void HandleKeyEvent(io::Key key);
    void HandleMouseMotion(int x, int y);
    void HandleMouseButton(io::Button button);
    void HandleCloseEvent();
    void ActivateSelectedButton();

private:
    // Event handles (RAII auto-unsubscribe on destruction)
    core::EventHandle m_CloseHandle;
    core::EventHandle m_KeyHandle;
    core::EventHandle m_MouseMotionHandle;
    core::EventHandle m_MouseButtonHandle;

    // Resources
    gfx::BitmapLoader m_Loader;
    gfx::Bitmap m_MenuBackground = nullptr;

    // State
    bool m_ShouldExit = false;
    int m_SelectedButton = 0;
    int m_MouseX = 0;
    int m_MouseY = 0;
    int m_StartupFrames = 0;  // Ignore input during startup

    // Button configuration
    static constexpr int BUTTON_COUNT = 3;
    menu::Button m_Buttons[BUTTON_COUNT];
};
