#pragma once

#ifdef WIN32
#include "win32_platform.hpp"
#endif

#include <cstdint>

struct GameMemory
{
    // At least 4GB
    size_t size;
    void *memory;
};

typedef float gFloat;

struct Color
{
    float red;
    float green;
    float blue;

    constexpr Color(float r, float g, float b) : red(r), green(g), blue(b) {}
};

struct RenderingInfo
{
    int width;
    int height;

    Color *bytes;
};

#define GAME_INITIALIZE(name) void name(GameMemory memory)
typedef GAME_INITIALIZE(game_initialize_function);

#define GAME_PROCESS_TICK(name) void name(GameMemory memory, int ticksElapsed)
typedef GAME_PROCESS_TICK(game_process_tick_function);

#define GAME_PREPARE_FOR_RENDERER(name) void name(GameMemory memory, RenderingInfo renderingInfo)
typedef GAME_PREPARE_FOR_RENDERER(game_prepare_for_renderer_function);
