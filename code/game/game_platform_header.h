#pragma once

#include <cstddef>
#include <cstdint>

struct DisplayBuffer {
  size_t width;
  size_t height;
  double dpi;
  uint8_t *bitmap; // RR GG BB AA
};

#define GAME_UPDATE_AND_RENDER(name) void name(DisplayBuffer *displayBuffer)
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

#define GAME_PROCESS_EVENT(name) void name(UserInputEvent *event)
typedef GAME_PROCESS_EVENT(game_process_event_function);
