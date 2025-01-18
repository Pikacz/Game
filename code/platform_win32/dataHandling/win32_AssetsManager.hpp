#pragma once
#include "platform.hpp"

#include <wchar.h>

#include "Windows.h"

#define WCHAR_AND_SIZE(wstr) wstr, (sizeof(wstr) / sizeof(wchar_t)) - 1

wchar_t *PreparePath(const wchar_t *prefix, size_t prefixLength, const char *path, size_t pathLength)
{
    LOG("Prefix: %ls prefix size: %zd\n", prefix, prefixLength);
    size_t bufferSize = (prefixLength + 1 + pathLength) * sizeof(wchar_t);
    wchar_t *result = (wchar_t *)LocalAlloc(LPTR, bufferSize);

    wmemcpy_s(result, bufferSize, prefix, prefixLength);

    if (!MultiByteToWideChar(CP_UTF8, 0, path, pathLength, result + prefixLength, pathLength))
    {
        LOG_LAST_ERROR("Unable to prepare path for %s\n", path);
    }

    for (size_t i = 0; i < bufferSize; ++i)
    {
        if (result[i] == '/')
        {
            result[i] = '\\';
        }
    }

    return result;
}

ASSETS_MANAGER_OPEN_FILE_AND_GET_SIZE(AssetsManagerOpenFileAndGetSize)
{
    wchar_t *path = PreparePath(WCHAR_AND_SIZE(L"assets\\"), assetName, nameSize);
    AssetsManagerFileHandle managerHandle;

    managerHandle._file_Handle = CreateFileW(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    managerHandle.size = 0;

    if (managerHandle._file_Handle == INVALID_HANDLE_VALUE)
    {
        LOG_LAST_ERROR("Unable to open asset %s. Path %ls\n", assetName, path);
        return managerHandle;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(managerHandle._file_Handle, &fileSize))
    {
        LOG_LAST_ERROR("Unable to get asset %s size. Path %ls\n", assetName, path);
        CloseHandle(managerHandle._file_Handle);
        managerHandle._file_Handle = INVALID_HANDLE_VALUE;
        return managerHandle;
    }
    LocalFree(path);
    managerHandle.size = fileSize.QuadPart;

    return managerHandle;
}

ASSETS_MANAGER_READ_FILE_AND_CLOSE(AssetsManagerReadFileAndClose)
{
    DWORD bytesRead = 0;
    if (!ReadFile(handle._file_Handle, destination, static_cast<DWORD>(handle.size), &bytesRead, nullptr))
    {
        LOG_LAST_ERROR("Unable to read file!\n");
    }
    else if (bytesRead != (DWORD)handle.size)
    {
        LOG("Successful read yet not completed.\n");
    }
    CloseHandle(handle._file_Handle);
}