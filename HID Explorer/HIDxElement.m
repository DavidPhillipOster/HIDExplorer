//
//  HIDxElement.m
//  HID Explorer
//
//  Created by david on 8/11/13.
//  Copyright (c) 2013 David Phillip Oster. All rights reserved.
//

#import "HIDxElement.h"

@interface HIDxElement ()
@property(nonatomic) NSInteger integerValue;
@property IOHIDElementType elementType;
@end

@implementation HIDxElement

- (id)initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    [self hidxElementInit];
  }
  return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder{
  self = [super initWithCoder:aDecoder];
  if (self) {
    [self hidxElementInit];
  }
  return self;
}

- (void)hidxElementInit {
}

- (void)drawRect:(NSRect)dirtyRect {
  NSRect bounds = [self bounds];
  [NSColor.whiteColor set];
  NSRectFill(bounds);
  [NSColor.blackColor set];
  NSFrameRect(bounds);
  if (_integerMin < _integerMax && _integerMin <= _integerValue && _integerValue <= _integerMax) {
    if (_integerValue) {
      CGRect inner = CGRectInset(bounds, 2, 2);
      inner.size.width *= (_integerValue - _integerMin)/(double)(_integerMax - _integerMin);
      [NSColor.darkGrayColor set];
      NSRectFill(inner);
    }
  }
}

- (void)setElement:(IOHIDElementRef)element {
  if (_element != element) {
    _element = element;
    _integerValue = -1;
    _elementType = IOHIDElementGetType(element);
  }
}

- (void)setIntegerValue:(NSInteger)integerValue {
  if (_integerValue != integerValue) {
    _integerValue = integerValue;
    [self setNeedsDisplay:YES];
  }
}

- (void)setIntegerMax:(NSInteger)integerMax {
  if (_integerMax != integerMax) {
    _integerMax = integerMax;
    [self setNeedsDisplay:YES];
  }
}

- (void)setIntegerMin:(NSInteger)integerMin {
  if (_integerMin != integerMin) {
    _integerMin = integerMin;
    [self setNeedsDisplay:YES];
  }
}

- (void)poll {
  if (kIOHIDElementTypeInput_Misc <= _elementType && _elementType <= kIOHIDElementTypeInput_Axis && NULL != _element) {
    NSInteger value = IOHIDElement_GetValue(_element, kIOHIDValueScaleTypePhysical);
    [self setIntegerValue:value];
  }
}

@end
