#pragma once

#include <cstddef>
#include <cstdint>

struct DisplayBuffer {
  size_t width;
  size_t height;
  double dpi;
  uint8_t *bitmap; // RR GG BB AA
};

struct GameMemory {
  size_t size;
  uint8_t memory[];
};

#define GAME_INIT_MEMORY(name) void name(GameMemory *memory)
typedef GAME_INIT_MEMORY(game_init_memory_function);

#define GAME_UPDATE_AND_RENDER(name)                                           \
  void name(DisplayBuffer *displayBuffer, GameMemory *memory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render_function);

enum UserInputEventType {
  UserInputEventMouseMove,
  UserInputEventMouseButtonsChanges,

  UserInputEventKeyDown,
  UserInputEventKeyUp,

  UserInputCharacterDown,
};

union UserInputEventValue {
  uint16_t keyboardKey;
  const char *character;
  struct {
    uint16_t x;
    uint16_t y;
    uint16_t selectedButtons;
  } cursorPosition;
};

struct UserInputEvent {
  UserInputEventType type;
  UserInputEventValue value;
};

#define GAME_PROCESS_EVENT(name)                                               \
  void name(UserInputEvent *event, GameMemory *memory)
typedef GAME_PROCESS_EVENT(game_process_event_function);
