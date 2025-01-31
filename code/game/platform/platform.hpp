#pragma once

#ifdef WIN32
#include "win32_platform.hpp"
#endif

#include <cstdint>


void crash()
{
    uint8_t *ptr = 0;
    *ptr = 13;
}

#define TEXT_AND_SIZE(str) str, sizeof(str) - 1

#define FILE_MANAGER_OPEN_FILE_AND_GET_SIZE(name) void name(const char *assetName, size_t nameSize, FileManagerFileHandle *fileHandle)
typedef FILE_MANAGER_OPEN_FILE_AND_GET_SIZE(FILE_MANAGER_open_file_and_get_size_function);

#define FILE_MANAGER_READ_FILE_AND_CLOSE(name) void name(FileManagerFileHandle handle, void *destination)
typedef FILE_MANAGER_READ_FILE_AND_CLOSE(FILE_MANAGER_read_file_and_close_function);

struct FileManager
{
    FILE_MANAGER_open_file_and_get_size_function *OpenFileAndGetSize;
    FILE_MANAGER_read_file_and_close_function *ReadFileAndClose;
};

struct HeapStack
{
    uint8_t *memory;
    ptrdiff_t topOffset;
    ptrdiff_t limit;

    uint8_t *push(size_t size)
    {
        uint8_t *result = memory + topOffset;
        topOffset += size;
        return result;
    }

    void pop(size_t size)
    {
        topOffset -= size;
    }
};

struct PlatformLayer
{
    /// 2GB stack
    HeapStack gameStateMemory;
    /// 2GB stack
    HeapStack assetsMemory;
    FileManager fileManager;
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

#define GAME_INITIALIZE(name) void name(PlatformLayer *platformLayer)
typedef GAME_INITIALIZE(game_initialize_function);

#define GAME_PROCESS_TICK(name) void name(PlatformLayer *platformLayer, int ticksElapsed)
typedef GAME_PROCESS_TICK(game_process_tick_function);

#define GAME_PREPARE_FOR_RENDERER(name) void name(PlatformLayer *platformLayer, RenderingInfo renderingInfo)
typedef GAME_PREPARE_FOR_RENDERER(game_prepare_for_renderer_function);
