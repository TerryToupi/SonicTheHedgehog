#pragma once

#include "Core/Context.h"
#include "Core/EventRegistry.h"

class CreditsScene : public core::Context
{
public:
    void Initialize() override;
    void Load() override;
    void Clear() override;

private:
    void OnRender();
    void OnInput();
    void HandleKeyEvent(io::Key key);
    void HandleMouseButton(io::Button button);
    void HandleCloseEvent();

private:
    core::EventHandle m_CloseHandle;
    core::EventHandle m_KeyHandle;
    core::EventHandle m_MouseButtonHandle;

    bool m_ShouldExit = false;
    int m_FlashCounter = 0;
};
