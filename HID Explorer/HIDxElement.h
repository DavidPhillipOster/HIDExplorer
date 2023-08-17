//
//  HIDxElement.h
//  HID Explorer
//
//  Created by david on 8/11/13.
//  Copyright (c) 2013 David Phillip Oster. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IOHIDElement_.h"

@interface HIDxElement : NSView
@property(nonatomic) IOHIDElementRef element;
@property(nonatomic) NSInteger integerMin;
@property(nonatomic) NSInteger integerMax;

- (void)poll;

@end
