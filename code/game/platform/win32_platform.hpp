#pragma once

#include <cstdio>
#include <cstdlib>

#define SLOW 1

#define DLL_EXPORT __declspec(dllexport)

#define LOG(message, ...) printf(message, ##__VA_ARGS__)

#define FAIL(message, ...)           \
    {                                \
        LOG(message, ##__VA_ARGS__); \
        exit(1);                     \
    }

#define LOG_LAST_ERROR(message, ...)                                                                  \
    {                                                                                                 \
        DWORD error = GetLastError();                                                                 \
        char *errorMessage = nullptr;                                                                 \
        FormatMessageA(                                                                               \
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,                              \
            nullptr,                                                                                  \
            error,                                                                                    \
            0,                                                                                        \
            (LPSTR) & errorMessage,                                                                   \
            0,                                                                                        \
            nullptr);                                                                                 \
                                                                                                      \
        int subMessageSize = snprintf(NULL, 0, message, ##__VA_ARGS__) + 1;                           \
        char *subBuffer = (char *)malloc(subMessageSize * sizeof(char));                              \
        snprintf(subBuffer, subMessageSize, message, ##__VA_ARGS__);                                  \
        int messageSize = snprintf(NULL, 0, "%serror: %lu %s\n", subBuffer, error, errorMessage) + 1; \
        char *buffer = (char *)malloc(messageSize * sizeof(char));                                    \
        snprintf(buffer, messageSize, "%serror: %lu %s\n", subBuffer, error, errorMessage);           \
        LocalFree(errorMessage);                                                                      \
        LOG("%s", buffer);                                                                            \
        free(buffer);                                                                                 \
    }

#define FAIL_LAST_ERROR(message, ...)           \
    {                                           \
        LOG_LAST_ERROR(message, ##__VA_ARGS__); \
        exit(1);                                \
    }