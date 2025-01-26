#pragma once

#include "platform.hpp"

#include "Windows.h"

static constexpr SIZE_T GameMemory_oneGB =  (1LL << 30);

static constexpr SIZE_T GameMemory_gameStateSize = 2 * GameMemory_oneGB;
static constexpr SIZE_T GameMemory_AssetsSize = 2 * GameMemory_oneGB;

void GameMemoryInit(PlatformLayer *platformLayer)
{
    platformLayer->gameStateMemory.memory = (uint8_t *)VirtualAlloc(NULL, GameMemory_gameStateSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    platformLayer->gameStateMemory.topOffset = 0;
    platformLayer->gameStateMemory.limit = GameMemory_gameStateSize;
    if (platformLayer->gameStateMemory.memory == nullptr) {
        LOG_LAST_ERROR("Alloc failed\n");
        crash();
    }

    platformLayer->assetsMemory.memory = (uint8_t *)VirtualAlloc(NULL, GameMemory_AssetsSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    platformLayer->assetsMemory.topOffset = 0;
    platformLayer->assetsMemory.limit = GameMemory_AssetsSize;
    if (platformLayer->gameStateMemory.memory == nullptr) {
        LOG_LAST_ERROR("Alloc failed\n");
        crash();
    }
    
}

void GameMemoryRelease(PlatformLayer *platformLayer)
{
    VirtualFree(platformLayer->gameStateMemory.memory, GameMemory_gameStateSize, MEM_RELEASE);
    VirtualFree(platformLayer->assetsMemory.memory, GameMemory_AssetsSize, MEM_RELEASE);
}