#import <AppKit/AppKit.h>
#include <Foundation/Foundation.h>

#include "GameWindow.h"

inline void SetWorkingDirectory() {
  NSString *executablePath = [[NSBundle mainBundle] executablePath];
  NSString *executableDirectory =
      [executablePath stringByDeletingLastPathComponent];
  [[NSFileManager defaultManager]
      changeCurrentDirectoryPath:executableDirectory];
}

@interface GameMain : NSObject <NSApplicationDelegate>
@property(strong, nonatomic) GameWindow *window;
@end

@implementation GameMain

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  self.window = [[GameWindow alloc] init];
  [self.window makeKeyAndOrderFront:nil];
}
@end

int main(int argc, const char *argv[]) {
  @autoreleasepool {
    SetWorkingDirectory();

    GameMain *delegate = [[GameMain alloc] init];
    [NSApplication sharedApplication].delegate = delegate;

    ProcessSerialNumber psn = {0, kCurrentProcess};
    OSStatus status =
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);

    [NSApp run];

    [NSApplication sharedApplication];
  }

  return 0;
}