#pragma once

#include "Core/Context.h"
#include "Core/EventRegistry.h"
#include "Rendering/Bitmap.h"
#include "Rendering/Clipper.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"

#include "Sprites/Ring.h"

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

private:
    // Event handles
    core::EventHandle m_CloseHandle;
    core::EventHandle m_KeyHandle;

    // Resources
    gfx::BitmapLoader m_Loader;
    scene::TileLayer m_TileLayer;
    scene::GridMap m_Grid;

    // Camera state
    int m_CameraX = 0;
    int m_CameraY = 0;

    // Display state
    bool m_ShowGrid = false;
    bool m_ShouldExit = false;

    // Sprites
    std::vector<Ring*> m_Rings;
    gfx::Clipper m_Clipper;

    // Level constants
    static constexpr int LEVEL_WIDTH = 10240;
    static constexpr int LEVEL_HEIGHT = 1536;
    static constexpr int SCROLL_SPEED = 4;
    static constexpr int GRID_Y_OFFSET = 256;
};
