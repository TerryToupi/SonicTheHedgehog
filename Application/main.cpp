#include "Scenes/SceneManager.h"

int main(void)
{
    // Window: 1280x720, Viewport: 320x224 (classic Sega Genesis resolution)
    SceneManager::Get().Initialize("Sonic Level Viewer", 1280, 720, 320, 224);

    SceneManager::Get().Run();  // Runs scene loop until EXIT

    SceneManager::Get().Shutdown();

    return 0;
}
