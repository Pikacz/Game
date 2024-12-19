#pragma once

#import <AppKit/AppKit.h>
#include <AppKit/NSEvent.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <cstddef>

#include "GameDynamicLib.h"
#include "GameView.h"
#include "game_platform_header.h"

@interface GameWindow : NSWindow <NSWindowDelegate>
@property(strong, nonatomic) GameView *mainView;
@property(nonatomic, strong) NSTimer *timer;

- (instancetype)init;
@end

@implementation GameWindow {
  struct GameDynamicLib _gameLib;
  uint16_t _mouseButtons;
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
  _mouseButtons = 0;
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

- (void)sendEvent:(NSEvent *)event {
  UserInputEvent gameEvent;
  switch ([event type]) {
  case NSEventTypeKeyDown: {
    gameEvent.type = UserInputCharacterDown;
    gameEvent.value.character =
        [[event characters] cStringUsingEncoding:NSUTF8StringEncoding];
    _gameLib.GameProcessEvent(&gameEvent);

    gameEvent.type = UserInputEventKeyDown;
    gameEvent.value.keyboardKey = [event keyCode];
    _gameLib.GameProcessEvent(&gameEvent);
    return;
  } break;
  case NSEventTypeKeyUp: {
    gameEvent.type = UserInputEventKeyUp;
    gameEvent.value.keyboardKey = [event keyCode];
    _gameLib.GameProcessEvent(&gameEvent);
    return;
  } break;
  case NSEventTypeFlagsChanged: {
    gameEvent.value.keyboardKey = [event keyCode];
    switch ([event keyCode]) {
    case 61:
    case 58: {
      if ((event.modifierFlags & NSEventModifierFlagOption) != 0) {
        gameEvent.type = UserInputEventKeyDown;
      } else {
        gameEvent.type = UserInputEventKeyUp;
      }
    } break;
    case 57: {
      if ((event.modifierFlags & NSEventModifierFlagCapsLock) != 0) {
        gameEvent.type = UserInputEventKeyDown;
      } else {
        gameEvent.type = UserInputEventKeyUp;
      }
    } break;
    case 56:
    case 60: {
      if ((event.modifierFlags & NSEventModifierFlagShift) != 0) {
        gameEvent.type = UserInputEventKeyDown;
      } else {
        gameEvent.type = UserInputEventKeyUp;
      }
    } break;
    case 59: {
      if ((event.modifierFlags & NSEventModifierFlagControl) != 0) {
        gameEvent.type = UserInputEventKeyDown;
      } else {
        gameEvent.type = UserInputEventKeyUp;
      }
    } break;
    case 54:
    case 55: {
      if ((event.modifierFlags & NSEventModifierFlagCommand) != 0) {
        gameEvent.type = UserInputEventKeyDown;
      } else {
        gameEvent.type = UserInputEventKeyUp;
      }
    } break;
    case 63: {
      // We do not care about fn key
    } break;
    default: {
      NSLog(@"changed TODO %u", [event keyCode]);
    } break;
    }
    _gameLib.GameProcessEvent(&gameEvent);
  } break;
  case NSEventTypeLeftMouseDragged:
  case NSEventTypeRightMouseDragged:
  case NSEventTypeOtherMouseDragged: {
    _mouseButtons |= (1 << [event buttonNumber]);
    [self processMouseAt:[event locationInWindow] gameEvent:&gameEvent.value];
    gameEvent.type = UserInputEventMouseMove;
    _gameLib.GameProcessEvent(&gameEvent);
  } break;
  case NSEventTypeMouseMoved: {
    _mouseButtons = 0;
    [self processMouseAt:[event locationInWindow] gameEvent:&gameEvent.value];
    gameEvent.type = UserInputEventMouseMove;
    _gameLib.GameProcessEvent(&gameEvent);
  } break;
  case NSEventTypeMouseEntered: {
    _mouseButtons = 0;
  } break;
  case NSEventTypeMouseExited: {
    _mouseButtons = 0;
  } break;
  case NSEventTypeRightMouseDown:
  case NSEventTypeLeftMouseDown:
  case NSEventTypeOtherMouseDown: {
    _mouseButtons |= (1 << [event buttonNumber]);
    [self processMouseAt:[event locationInWindow] gameEvent:&gameEvent.value];
    gameEvent.type = UserInputEventMouseButtonsChanges;
    _gameLib.GameProcessEvent(&gameEvent);
  } break;

  case NSEventTypeLeftMouseUp:
  case NSEventTypeRightMouseUp:
  case NSEventTypeOtherMouseUp: {
    _mouseButtons &= ~(1 << [event buttonNumber]);
    [self processMouseAt:[event locationInWindow] gameEvent:&gameEvent.value];
    gameEvent.type = UserInputEventMouseButtonsChanges;
    _gameLib.GameProcessEvent(&gameEvent);
  } break;
  case NSEventTypePressure: {
    // TODO: Do i care about trackpad?
  } break;
  case NSEventTypeAppKitDefined:
    break;
  default: {
    NSLog(@"something else %lu\n", static_cast<unsigned long>([event type]));
  } break;
  }
  [super sendEvent:event];
}

- (void)processMouseAt:(NSPoint)cursorLocation
             gameEvent:(UserInputEventValue *)gameEventValue {

  NSPoint pointInView = [self.mainView convertPoint:cursorLocation
                                           fromView:NULL];
  gameEventValue->cursorPosition.selectedButtons = _mouseButtons;
  gameEventValue->cursorPosition.x = (uint16_t)fmax(
      0.0f, round(pointInView.x / self.mainView.frame.size.width *
                  (CGFloat)self.mainView.displayBuffer->width));
  gameEventValue->cursorPosition.y = (uint16_t)fmax(
      0.0f, round(pointInView.y / self.mainView.frame.size.height *
                  (CGFloat)self.mainView.displayBuffer->height));
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