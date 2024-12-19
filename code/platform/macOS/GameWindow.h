#pragma once

#import <AppKit/AppKit.h>

#include "GameDynamicLib.h"
#include "GameView.h"

@interface GameWindow : NSWindow <NSWindowDelegate>
@property(strong, nonatomic) GameView *mainView;
@property(nonatomic, strong) NSTimer *timer;

- (instancetype)init;
@end

@implementation GameWindow {
  struct GameDynamicLib _gameLib;
}

- (instancetype)init {

  self = [super
      initWithContentRect:(NSMakeRect(0, 0, 800, 600))
                styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable)
                  backing:NSBackingStoreBuffered
                    defer:NO];

  [self setTitle:@"Game"];
  self.delegate = self;
  self.mainView = [[GameView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
  self.contentView = self.mainView;

  _gameLib.Initialize();
  _gameLib.LoadIfNeeded(GAME_DLIB);
  _gameLib.GameUpdateAndRender(self.mainView.displayBuffer);
  self.mainView.needsDisplay = true;

  self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0f / 60.0f
                                                target:self
                                              selector:@selector(gameTick)
                                              userInfo:nil
                                               repeats:YES];

  return self;
}

- (void)gameTick {
#if GAME_SLOW
  _gameLib.LoadIfNeeded(GAME_DLIB);
#endif
  _gameLib.GameUpdateAndRender(self.mainView.displayBuffer);
  self.mainView.needsDisplay = true;
}

#pragma mark GameWindow - NSWindowDelegate

- (void)windowWillClose:(NSNotification *)notification {
  [NSApp stop:NULL];
}
@end