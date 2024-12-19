#pragma once

#import <AppKit/AppKit.h>

#include "Logging.h"
#include "game_platform_header.h"

DisplayBuffer *CreateDisplayBuffer(size_t width, size_t height, double dpi) {
  vm_address_t address = 0;
  kern_return_t result =
      vm_allocate(mach_task_self(), &address,
                  sizeof(DisplayBuffer) + width * height * sizeof(uint8_t) * 4,
                  VM_FLAGS_ANYWHERE);
  if (result != KERN_SUCCESS) {
    LOG_ERROR("Unable to allocate Display Buffer. Error %d (%s)\n", result,
              mach_error_string(result));
  }
  DisplayBuffer *buffer = (DisplayBuffer *)address;
  buffer->dpi = dpi;
  buffer->width = width;
  buffer->height = height;
  buffer->bitmap = (uint8_t *)(buffer + 1);
  return buffer;
}

void FreeDisplayBuffer(DisplayBuffer *buffer) {
  kern_return_t result =
      vm_deallocate(mach_task_self(), (vm_address_t)buffer,
                    sizeof(DisplayBuffer) +
                        buffer->width * buffer->height * sizeof(uint8_t) * 4);
  if (result != KERN_SUCCESS) {
    LOG_ERROR("Unable to deallocate Display Buffer. Error %d (%s)\n", result,
              mach_error_string(result));
  }
}

@interface GameView : NSView
@property(nonnull, nonatomic) DisplayBuffer *displayBuffer;
@end

@implementation GameView

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super initWithCoder:coder];
  if (self) {
    [self setupBuffer];
  }
  return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
  self = [super initWithFrame:frameRect];
  if (self) {
    [self setupBuffer];
  }
  return self;
}

- (void)setupBuffer {
  self.displayBuffer = CreateDisplayBuffer(1600, 900, 1.0f);
}

- (void)dealloc {
  FreeDisplayBuffer(self.displayBuffer);
  [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
  [super drawRect:dirtyRect];
  CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGContextRef bitmapContext = CGBitmapContextCreate(
      self.displayBuffer->bitmap, self.displayBuffer->width,
      self.displayBuffer->height, sizeof(uint8_t) * 8,
      4 * sizeof(uint8_t) * self.displayBuffer->width, colorSpace,
      kCGImageAlphaPremultipliedLast);
  CGImageRef image = CGBitmapContextCreateImage(bitmapContext);
  CGContextDrawImage(context, self.bounds, image);
  CGImageRelease(image);
  CGContextRelease(bitmapContext);
  CGColorSpaceRelease(colorSpace);
}
@end