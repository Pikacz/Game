#include "game_platform_header.h"

#include <cstdio>

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
  printf("GameUpdateAndRender\n");
}
