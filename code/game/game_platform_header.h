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
