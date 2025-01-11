#pragma once

#ifdef WIN32
#include "win32_platform.hpp"
#endif

#define GAME_DO_SOMETHING(name) void name()
typedef GAME_DO_SOMETHING(game_do_something_function);