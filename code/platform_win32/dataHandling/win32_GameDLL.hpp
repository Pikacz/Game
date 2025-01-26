#pragma once
#include "platform.hpp"
#include "Windows.h"

#include <ctime>

time_t GetFileModificationDate(const char *path)
{
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    struct tm tm = {0};
    time_t result = 0;
    FILETIME ft;

    if (hFile == INVALID_HANDLE_VALUE)
    {
        LOG_LAST_ERROR("Unable to open file %s\n", path);
        return 0;
    }

    if (!GetFileTime(hFile, NULL, NULL, &ft))
    {
        LOG_LAST_ERROR("Unable to GetFileTime from %s\n", path);
        goto return_result;
    }
    SYSTEMTIME stUTC, stLocal;
    if (!FileTimeToSystemTime(&ft, &stUTC))
    {
        LOG_LAST_ERROR("Unable to FileTimeToSystemTime from %s\n", path);
        goto return_result;
    }
    if (!SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal))
    {
        LOG_LAST_ERROR("Unable to SystemTimeToTzSpecificLocalTime from %s\n", path);
        goto return_result;
    }

    tm.tm_year = stLocal.wYear - 1900;
    tm.tm_mon = stLocal.wMonth - 1;
    tm.tm_mday = stLocal.wDay;
    tm.tm_hour = stLocal.wHour;
    tm.tm_min = stLocal.wMinute;
    tm.tm_sec = stLocal.wSecond;
    result = mktime(&tm);

return_result:
    CloseHandle(hFile);
    return result;
}

void CopyOverrideFile(const char *source, const char *destination)
{
    if (!CopyFile(source, destination, FALSE))
    {
        LOG_LAST_ERROR("Unable to copy %s to %s.\n", source, destination);
    }
}

#if SLOW
static constexpr const char *_DLL_TO_LOAD = "loaded_game.dll";
#else
static constexpr const char *_DLL_TO_LOAD = "game.dll";
#endif

struct GameDLL
{
    GAME_INITIALIZE(GameInitialize)
    {
#if SLOW
        if (gameInitialize)
        {
            gameInitialize(platformLayer);
        }
        else
        {
            LOG("Trying to call GameInitialize without DLL loaded\n");
        }
#else
        gameInitialize(platformLayer);
#endif
    }

    GAME_PROCESS_TICK(GameProcessTick)
    {
#if SLOW
        if (gameProcessTick)
        {
            gameProcessTick(platformLayer, ticksElapsed);
        }
        else
        {
            LOG("Trying to call GameProcessTick without DLL loaded\n");
        }
#else
        gameProcessTick(platformLayer, ticksElapsed);
#endif
    }

    GAME_PREPARE_FOR_RENDERER(GamePrepareForRenderer)
    {
#if SLOW
        if (gamePrepareForRenderer)
        {
            gamePrepareForRenderer(platformLayer, renderingInfo);
        }
        else
        {
            LOG("Trying to call GamePrepareForRenderer without DLL loaded\n");
        }
#else
        gamePrepareForRenderer(platformLayer, renderingInfo);
#endif
    }

#if SLOW
    void LoadIfNeeded()
    {
        time_t currentModification = GetFileModificationDate("game.dll");
        if (gameDLL == NULL || lastModification < currentModification)
        {
            if (gameDLL)
            {
                if (!FreeLibrary(gameDLL))
                {
                    LOG_LAST_ERROR("Unable to free game dll.\n");
                }
            }
            memset(this, 0, sizeof(GameDLL));
            Load();
            lastModification = currentModification;
        }
    }
#endif

    void Load()
    {
#if SLOW
        CopyOverrideFile("game.dll", _DLL_TO_LOAD);
#endif
        gameDLL = LoadLibrary(_DLL_TO_LOAD);
        if (!gameDLL)
        {
            LOG_LAST_ERROR("Unable to load %s\n", _DLL_TO_LOAD);
            return;
        }
        gameInitialize = (game_initialize_function *)(GetProcAddress(gameDLL, "GameInitialize"));
        if (!gameInitialize)
        {
            LOG_LAST_ERROR("Unable to load GameInitialize\n");
        }
        gameProcessTick = (game_process_tick_function *)(GetProcAddress(gameDLL, "GameProcessTick"));
        if (!gameProcessTick)
        {
            LOG_LAST_ERROR("Unable to load GameProcessTick\n");
        }
        gamePrepareForRenderer = (game_prepare_for_renderer_function *)(GetProcAddress(gameDLL, "GamePrepareForRenderer"));
        if (!gamePrepareForRenderer)
        {
            LOG_LAST_ERROR("Unable to load GamePrepareForRenderer\n");
        }
    }

    HMODULE gameDLL;
    time_t lastModification;
    game_initialize_function *gameInitialize;
    game_process_tick_function *gameProcessTick;
    game_prepare_for_renderer_function *gamePrepareForRenderer;
};