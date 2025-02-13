#include "platform/platform.hpp"
#include "game_drawing.hpp"
#include "fontEngine/trueType.h"
#include "fontEngine/gameFont.h"

#include <numeric>
#include <algorithm>

constexpr int boardHeight = 180;
constexpr int boardWidth = 320;
constexpr int glyphHeight = 16;
constexpr int glyphWidth = 16;
struct Board
{
    int fields[boardHeight][boardWidth];
    int playerX;
    int playerY;
    int8_t glyphBitmap[glyphHeight * glyphWidth];
};

Board *getBoard(HeapStack memory)
{
    return (Board *)memory.memory;
}

extern "C" DLL_EXPORT GAME_INITIALIZE(GameInitialize)
{
    // LoadFont(TEXT_AND_SIZE("test_scrapped_assets/simplex.ttf"), platformLayer);
    // LoadFont(TEXT_AND_SIZE("test_scrapped_assets/Roboto-Medium.ttf"), platformLayer);
    Board *board = getBoard(platformLayer->gameStateMemory);
    for (int i = 0; i < boardWidth; ++i)
    {
        board->fields[0][i] = 1;
        board->fields[boardHeight - 1][i] = 1;
    }
    for (int i = 1; i < boardHeight - 1; ++i)
    {
        board->fields[i][0] = 1;
        board->fields[i][boardWidth - 1] = 1;
    }
    board->playerX = 160;
    board->playerY = 90;

    Section arrows[14];
    int arrowCount[2] = { 4, 8 };


    // TODO: Fix these artifacts!
    arrows[0] = {  1.1, 14.1, 13.9, 14.1 };
    arrows[1] = { 13.9, 14.1, 13.9,  2.1 };
    arrows[2] = { 13.9,  2.1,  1.1,  2.1 };
    arrows[3] = { 1.1,   2.1,  1.1, 14.1 };

    arrows[4] = { 9.5, 11.1, 3.1, 11.1, };
    arrows[5] = {  9.5,  4.1, 9.5, 11.1, };
    arrows[6] = {   3.1,  4.1, 9.5,  4.1, };
    arrows[7] = {   3.1, 11.1, 3.1,   4.1, };

    // arrows[0] = {   1.5, 10.12, 4.123, 15.72 };
    // arrows[1] = { 4.123, 15.72,  8.37, 15.72 };
    // arrows[2] = {  8.37, 15.72, 11.74, 10.12 };
    // arrows[3] = { 11.74, 10.12, 11.74,  4.72 };
    // arrows[4] = { 11.74,  4.72,  8.37,  1.31 };
    // arrows[5] = {  8.37,  1.31, 4.123,  1.31 };
    // arrows[6] = { 4.123,  1.31,   1.5,  4.72 };
    // arrows[7] = {   1.5,  4.72,   1.5, 10.12 };


    drawGlyph(arrows, arrowCount, 2, board->glyphBitmap, glyphWidth, glyphHeight, platformLayer->gameStateMemory.memory + 200000);


}

extern "C" DLL_EXPORT GAME_PROCESS_TICK(GameProcessTick)
{
    // LOG("GameProcessTick\n");
}

extern "C" DLL_EXPORT GAME_PREPARE_FOR_RENDERER(GamePrepareForRenderer)
{
    Board *board = getBoard(platformLayer->gameStateMemory);

    gFloat x;
    gFloat width = (gFloat)renderingInfo.width / (gFloat)boardWidth;
    gFloat y = 0;
    gFloat height = (gFloat)renderingInfo.height / (gFloat)boardHeight;

    for (int row = 0; row < 180; row++)
    {
        x = 0.0;
        for (int column = 0; column < 320; ++column)
        {
            if (board->fields[row][column] == 1)
            {
                drawRect(renderingInfo, x, y, width, height, Color_black);
            }
            else
            {
                drawRect(renderingInfo, x, y, width, height, Color_white);
            }

            x += width;
        }
        y += height;
    }

    gFloat playerX = (gFloat)board->playerX / (gFloat)boardWidth * (gFloat)renderingInfo.width;
    gFloat playerY = (gFloat)board->playerY / (gFloat)boardHeight * (gFloat)renderingInfo.height;
    gFloat playerR = 4.0f * width;

    drawCircle(renderingInfo, playerX, playerY, playerR, Color_yellow);

    {
        constexpr int pointWidth = 16;
        constexpr int startX = 64;
        constexpr int startY = 64;
        int x, y;

        for (int gY = 0; gY < glyphHeight; gY++)
        {
            y = startY + gY * pointWidth;

            for (int gX = 0; gX < glyphWidth; gX++)
            {
                x = startX + gX * pointWidth;

                for (int yO = 0; yO < pointWidth; yO++)
                {
                    for (int xO = 0; xO < pointWidth; xO++)
                    {
                        const int coordX = x + xO;
                        const int coordY = y + yO;
                        const int idx = renderingInfo.width * coordY + coordX;
                        renderingInfo.bytes[idx] = Color_green;
                        
                    }
                }
            }
        }

        for (int gY = 0; gY < glyphHeight; gY++)
        {
            y = startY + (glyphHeight - gY) * pointWidth;

            for (int gX = 0; gX < glyphWidth; gX++)
            {
                x = startX + gX * pointWidth;

                for (int yO = 0; yO < pointWidth; yO++)
                {
                    for (int xO = 0; xO < pointWidth; xO++)
                    {
                        const int coordX = x + xO;
                        const int coordY = y + yO;
                        const int idx = renderingInfo.width * coordY + coordX;
                        if (board->glyphBitmap[gY * glyphWidth + gX]) {
                            renderingInfo.bytes[idx] = Color_red;
                        }
                    }
                }
            }
        }
    }
}