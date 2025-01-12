#pragma once
#include "platform/platform.hpp"
#include <algorithm>

constexpr Color Color_white = Color(1.0f, 1.0f, 1.0f);
constexpr Color Color_black = Color(0.0f, 0.0f, 0.0f);
constexpr Color Color_yellow = Color(1.0f, 1.0f, 0.0f);

void drawRect(RenderingInfo renderingInfo, gFloat x, gFloat y, gFloat width, gFloat height, Color color)
{
    int left = (int)floor(x);
    int top = (int)floor(y);
    left = std::max(0, left);
    top = std::max(0, top);

    int right = (int)ceil(x + width);
    int bottom = (int)ceil(y + height);
    right = std::min(renderingInfo.width, right);
    bottom = std::min(renderingInfo.height, bottom);

    int idx;
    for (int coordX = left; coordX < right; ++coordX)
    {
        for (int coordY = top; coordY < bottom; ++coordY)
        {
            idx = renderingInfo.width * coordY + coordX;
            renderingInfo.bytes[idx] = color;
        }
    }
}

void drawCircle(RenderingInfo renderingInfo, gFloat x, gFloat y, gFloat radius, Color color)
{
    int left = (int)(x - radius);
    int top = (int)(y - radius);
    left = std::max(0, left);
    top = std::max(0, top);

    int right = (int)(x + radius);
    int bottom = (int)(y + radius);
    right = std::min(renderingInfo.width, right);
    bottom = std::min(renderingInfo.height, bottom);

    int idx;
    gFloat rSquare = radius * radius;

    gFloat pointX, pointY;
    for (int coordX = left; coordX < right; ++coordX)
    {
        for (int coordY = top; coordY < bottom; ++coordY)
        {
            pointX = (gFloat)coordX;
            pointY = (gFloat)coordY;

            pointX -= x;
            pointX *= pointX;

            pointY -= y;
            pointY *= pointY;
            if (pointX + pointY <= rSquare)
            {
                idx = renderingInfo.width * coordY + coordX;
                renderingInfo.bytes[idx] = color;
            }
        }
    }
}
