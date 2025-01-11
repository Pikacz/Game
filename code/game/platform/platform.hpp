#pragma once

#ifdef WIN32
#include "win32_platform.hpp"
#endif

#include <cstdint>

struct GameMemory
{
};


union Pixel {
    uint32_t raw;
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };
};

struct RenderingInfo
{
    int width;
    int height;

    Pixel *bytes;
};

#define GAME_INITIALIZE(name) void name(GameMemory *memory)
typedef GAME_INITIALIZE(game_initialize_function);

#define GAME_PROCESS_TICK(name) void name(GameMemory *memory, int ticksElapsed)
typedef GAME_PROCESS_TICK(game_process_tick_function);

#define GAME_PREPARE_FOR_RENDERER(name) void name(GameMemory *memory, RenderingInfo *renderingInfo)
typedef GAME_PREPARE_FOR_RENDERER(game_prepare_for_renderer_function);
