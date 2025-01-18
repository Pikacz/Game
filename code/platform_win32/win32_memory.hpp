#pragma once

#include "platform.hpp"
#include "dataHandling\win32_AssetsManager.hpp"

#include "Windows.h"

void GameMemoryInit(GameMemory *memory)
{
    memory->assetsManager.OpenFileAndGetSize = AssetsManagerOpenFileAndGetSize;
    memory->assetsManager.ReadFileAndClose = AssetsManagerReadFileAndClose;
    memory->size = 4 * 1024 * 1024 * 1025;
    memory->memory = VirtualAlloc(NULL, memory->size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!memory->memory)
    {
        LOG_LAST_ERROR("Unable to allocate game memory!\n");
    }
}

void GameMemoryRelease(GameMemory *memory)
{
    VirtualFree(memory->memory, memory->size, MEM_RELEASE);
}