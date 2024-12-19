#pragma once

#include "Logging.h"
#include "game_platform_header.h"

#include <cstring>
#include <ctime>
#include <dlfcn.h>
#include <sys/stat.h>

struct GameDynamicLib {
  GAME_UPDATE_AND_RENDER(GameUpdateAndRender) {
    if (_GameUpdateAndRenderFunc) {
      _GameUpdateAndRenderFunc(displayBuffer);
    } else {
      LOG_ERROR("Unable to call GameUpdateAndRender\n");
    }
  }

  GAME_PROCESS_EVENT(GameProcessEvent) {
    if (_GameProcessEvent) {
      _GameProcessEvent(event);
    } else {
      LOG_ERROR("Unable to call GameUpdateAndRender\n");
    }
  }

  void Initialize() { memset(this, 0, sizeof(GameDynamicLib)); }

  void LoadIfNeeded(const char *path) {
    std::time_t currentTimestamp;
    struct stat fileStat;
    if (stat(path, &fileStat) == 0) {
      currentTimestamp = fileStat.st_mtime;
    } else {
      LOG_ERROR("Unable to get timestamp of %s\n", path);
      currentTimestamp = dLibTimestamp + 1;
    }
    if (currentTimestamp > dLibTimestamp) {
      _Load(path);
      dLibTimestamp = currentTimestamp;
    }
  }

  inline void _Load(const char *path) {
    if (dlibHandle) {
      dlclose(dlibHandle);
      dlibHandle = nullptr;
      _GameUpdateAndRenderFunc = nullptr;
    }
    dlibHandle = dlopen(path, RTLD_LAZY);
    if (!dlibHandle) {
      LOG_ERROR("Unable to load dLib at %s, error %s\n", path, dlerror());
      return;
    }

    _GameUpdateAndRenderFunc = (game_update_and_render_function *)dlsym(
        dlibHandle, "GameUpdateAndRender");
    if (!_GameUpdateAndRenderFunc) {
      LOG_ERROR(
          "Unable to load GameUpdateAndRender function from %s, error %s\n",
          path, dlerror());
    }

    _GameProcessEvent =
        (game_process_event_function *)dlsym(dlibHandle, "GameProcessEvent");
    if (!_GameProcessEvent) {
      LOG_ERROR("Unable to load GameProcessEvent function from %s, error %s\n",
                path, dlerror());
    }

    return;
  }

  std::time_t dLibTimestamp;
  void *dlibHandle;
  game_update_and_render_function *_GameUpdateAndRenderFunc;
  game_process_event_function *_GameProcessEvent;
};
