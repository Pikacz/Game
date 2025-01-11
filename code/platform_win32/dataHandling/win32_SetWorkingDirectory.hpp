#pragma once

#include "platform.hpp"

#include "Windows.h"

static LPSTR _GetExecutablePath()
{
    DWORD bufferSize = 4 * MAX_PATH;
    LPSTR buffer = static_cast<LPSTR>(LocalAlloc(LPTR, bufferSize * sizeof(CHAR)));

    while (true)
    {
        if (GetModuleFileNameA(NULL, buffer, bufferSize))
        {
            DWORD error = GetLastError();
            if (error == ERROR_INSUFFICIENT_BUFFER)
            {
                bufferSize *= 2;
                LocalFree(buffer);
                buffer = static_cast<LPSTR>(LocalAlloc(LPTR, bufferSize * sizeof(CHAR)));
            }
            else if (error)
            {
                SetLastError(error);
                LOG_LAST_ERROR("Failed to set working directory. Unable to find executable path.\n");
                LocalFree(buffer);
                return nullptr;
            }
            else
            {
                SIZE_T lastSlash = bufferSize - 1;
                for (SIZE_T idx = 0; idx < bufferSize; ++idx)
                {
                    if (buffer[idx] == '\\')
                    {
                        lastSlash = idx;
                    }
                }
                buffer[lastSlash] = '\0';
                return buffer;
            }
        }
        else
        {
            LOG_LAST_ERROR("Failed to set working directory. Unable to find executable path.\n");
            LocalFree(buffer);
            return nullptr;
        }
    }
}

void SetWorkingDirectory()
{
    LPSTR path = _GetExecutablePath();
    if (path)
    {
        if (!SetCurrentDirectoryA(path))
        {
            LOG_LAST_ERROR("Failed to set working directory to %s\n", path);
        }
        else
        {
            LOG("Working directory path set to %s\n", path);
        }
        LocalFree(path);
    }
    else
    {
        LOG("Not setting working directory.\n");
    }
}