#include "game_platform_header.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

struct KeyMap {
  uint16_t keyUp;
  uint16_t keyDown;
  uint16_t keyLeft;
  uint16_t keyRight;
};

struct GameState {
  KeyMap mapping;
  uint8_t board[11 * 11];
};

#define GetGameState() (GameState *)memory->memory

extern "C" GAME_INIT_MEMORY(GameInitMemory) {
  GameState *state = GetGameState();
  state->mapping.keyUp = 13;
  state->mapping.keyDown = 1;
  state->mapping.keyLeft = 0;
  state->mapping.keyRight = 2;
  memset(state->board, 0, sizeof(uint8_t) * 11 * 11);
}

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
    printf("process key down %d\n", event->value.keyboardKey);
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