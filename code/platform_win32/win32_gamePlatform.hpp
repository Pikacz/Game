#pragma once
#include "platform.hpp"

#include "platform/win32_memory.hpp"
#include "platform/win32_fileManager.hpp"

void Win32GamePlatformInit(PlatformLayer *platformLayer)
{
    GameMemoryInit(platformLayer);
    FileManagerInit(&platformLayer->fileManager);
}

void Win32GamePlatformRelease(PlatformLayer *platformLayer)
{
    FileManagerRelease(&platformLayer->fileManager);
    GameMemoryRelease(platformLayer);
}