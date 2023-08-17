//  AppDelegate.h
//  HID Explorer
//
//  Created by David Phillip Oster on 8/11/13.
//  Copyright (c) 2013 David Phillip Oster. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class HIDxController;

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet HIDxController *controller;

- (IBAction)rebuild:(id)sender;
@end
