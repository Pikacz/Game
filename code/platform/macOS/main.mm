#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <unistd.h>

#include <cstdio>

#include "GameDynamicLib.h"
#include "game_platform_header.h"

inline void SetWorkingDirectory() {
  NSString *executablePath = [[NSBundle mainBundle] executablePath];
  NSString *executableDirectory =
      [executablePath stringByDeletingLastPathComponent];
  [[NSFileManager defaultManager]
      changeCurrentDirectoryPath:executableDirectory];
}

int main(int argc, const char *argv[]) {
  @autoreleasepool {
    SetWorkingDirectory();
    GameDynamicLib gameLib = {};
    gameLib.LoadIfNeeded(GAME_DLIB);
    while (true) {
      gameLib.GameUpdateAndRender();
      sleep(1);
      gameLib.LoadIfNeeded(GAME_DLIB);
    }
  }

  return 0;
}