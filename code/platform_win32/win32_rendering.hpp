#pragma once

#include <Windows.h>
#include "platform.hpp"

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
};

struct Renderer
{
    RenderingInfo gameInfo;
    Pixel *pixels;
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

        pixels = static_cast<Pixel *>(LocalAlloc(LPTR, width * height * sizeof(Pixel)));
        gameInfo.bytes = static_cast<Color *>(LocalAlloc(LPTR, width * height * sizeof(Color)));
    }

    void Deinitialize()
    {
        LocalFree(pixels);
        LocalFree(gameInfo.bytes);
    }

    inline void _writeToPixel(Color color, Pixel *pixel)
    {
        pixel->red = (int)color.red * 255.0f;
        pixel->green = (int)color.green * 255.0f;
        pixel->blue = (int)color.blue * 255.0f;
    }

    void Present(HDC hdc, int windowWidth, int windowHeight)
    {
        for (int i = 0; i < totalSize; ++i)
        {
            _writeToPixel(gameInfo.bytes[i], &pixels[i]);
        }

        StretchDIBits(
            hdc,
            0, 0, windowWidth, windowHeight,
            0, 0, gameInfo.width, gameInfo.height,
            static_cast<void *>(pixels),
            &bitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY);
    }
};