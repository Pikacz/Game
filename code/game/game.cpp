#include "platform/platform.hpp"
#include "game_drawing.hpp"

#include <numeric>
#include <algorithm>

constexpr int boardHeight = 180;
constexpr int boardWidth = 320;
struct Board
{
    int fields[boardHeight][boardWidth];
    int playerX;
    int playerY;
};

Board *getBoard(GameMemory memory)
{
    return (Board *)memory.memory;
}

extern "C" DLL_EXPORT GAME_INITIALIZE(GameInitialize)
{
    Board *board = getBoard(memory);
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
}

extern "C" DLL_EXPORT GAME_PROCESS_TICK(GameProcessTick)
{
    // LOG("GameProcessTick\n");
}

extern "C" DLL_EXPORT GAME_PREPARE_FOR_RENDERER(GamePrepareForRenderer)
{
    Board *board = getBoard(memory);

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
}