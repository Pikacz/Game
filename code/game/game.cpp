#include "platform/platform.hpp"

extern "C" DLL_EXPORT GAME_INITIALIZE(GameInitialize)
{
    LOG("GameInitialize\n");
}

extern "C" DLL_EXPORT GAME_PROCESS_TICK(GameProcessTick)
{
    // LOG("GameProcessTick\n");
}

extern "C" DLL_EXPORT GAME_PREPARE_FOR_RENDERER(GamePrepareForRenderer)
{
    int totalSize = renderingInfo->height * renderingInfo->width;
    for (int i = 0; i < totalSize; ++i) {
        renderingInfo->bytes[i].red = i % 255;
    }

    for (int row = 0; row < renderingInfo->height; row++) {
        for (int column = 0; column < renderingInfo->width; column++) {
            renderingInfo->bytes[row * renderingInfo->width + column].red = row % 255;
        }
    }
}