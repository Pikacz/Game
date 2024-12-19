#include "game_platform_header.h"
#include <cstdio>

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

extern "C" GAME_PROCESS_EVENT(GameProcessEvent) {
  switch (event->type) {
  case UserInputCharacterDown: {
    printf("character: %s\n", event->value.character);
  } break;
  case UserInputEventKeyDown: {
    printf("process key down\n");
  } break;
  case UserInputEventKeyUp: {

  } break;
  case UserInputEventMouseMove: {

  } break;
  case UserInputEventMouseButtonsChanges: {
    printf("process mouse click\n");
  } break;
  }
}