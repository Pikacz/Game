#include "game_platform_header.h"

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
  uint8_t *iterator = displayBuffer->bitmap;
  for (int y = 0; y < displayBuffer->height; ++y) {
    for (int x = 0; x < displayBuffer->width; ++x) {
      *iterator++ = (y * 255) / (displayBuffer->height - 1);
      *iterator++ = 0;
      *iterator++ = 0;
      *iterator++ = 255;
    }
  }
}
