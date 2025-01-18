#pragma once

#ifdef WIN32
#include "win32_platform.hpp"
#endif

#include <cstdint>

#define TEXT_AND_SIZE(str) str, sizeof(str) - 1

#define ASSETS_MANAGER_OPEN_FILE_AND_GET_SIZE(name) AssetsManagerFileHandle name(const char *assetName, size_t nameSize)
typedef ASSETS_MANAGER_OPEN_FILE_AND_GET_SIZE(assets_manager_open_file_and_get_size_function);

#define ASSETS_MANAGER_READ_FILE_AND_CLOSE(name) void name(AssetsManagerFileHandle handle, void *destination)
typedef ASSETS_MANAGER_READ_FILE_AND_CLOSE(assets_manager_read_file_and_close_function);

struct AssetsManager
{
    assets_manager_open_file_and_get_size_function *OpenFileAndGetSize;
    assets_manager_read_file_and_close_function *ReadFileAndClose;
};

struct GameMemory
{
    AssetsManager assetsManager;

    // At least 4GB
    size_t size;
    void *memory;
};

typedef float gFloat;

union Pixel
{
    uint32_t raw;
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };

    constexpr Pixel(
        uint8_t red,
        uint8_t green,
        uint8_t blue,
        uint8_t alpha = 0.0f) : blue(blue), green(green), red(red), alpha(alpha) {}

    constexpr Pixel(
        float r,
        float g,
        float b,
        float alpha = 0.0f) : Pixel((uint8_t)(r * 255.0f),
                                    (uint8_t)(g * 255.0f),
                                    (uint8_t)(b * 255.0f),
                                    (uint8_t)(alpha * 255.0f)) {}
};

struct RenderingInfo
{
    int width;
    int height;

    Pixel *bytes;
};

#define GAME_INITIALIZE(name) void name(GameMemory memory)
typedef GAME_INITIALIZE(game_initialize_function);

#define GAME_PROCESS_TICK(name) void name(GameMemory memory, int ticksElapsed)
typedef GAME_PROCESS_TICK(game_process_tick_function);

#define GAME_PREPARE_FOR_RENDERER(name) void name(GameMemory memory, RenderingInfo renderingInfo)
typedef GAME_PREPARE_FOR_RENDERER(game_prepare_for_renderer_function);
