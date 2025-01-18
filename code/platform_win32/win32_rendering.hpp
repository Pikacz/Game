#pragma once

#include <Windows.h>
#include "platform.hpp"

struct Renderer
{
    RenderingInfo gameInfo;
    BITMAPINFO bitmapInfo;
    int totalSize;

    void Initialize(int width, int height)
    {
        gameInfo.width = width;
        gameInfo.height = height;

        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth = width;
        bitmapInfo.bmiHeader.biHeight = -height;
        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biCompression = BI_RGB;
        bitmapInfo.bmiHeader.biSizeImage = 0;

        totalSize = width * height;
        gameInfo.bytes = static_cast<Pixel *>(LocalAlloc(LPTR, width * height * sizeof(Pixel)));
    }

    void Deinitialize()
    {
        LocalFree(gameInfo.bytes);
    }

    void Present(HDC hdc, int windowWidth, int windowHeight)
    {
        StretchDIBits(
            hdc,
            0, 0, windowWidth, windowHeight,
            0, 0, gameInfo.width, gameInfo.height,
            static_cast<void *>(gameInfo.bytes),
            &bitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY);
    }
};