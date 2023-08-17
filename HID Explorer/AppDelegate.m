//  AppDelegate.m
//  HID Explorer
//
//  Created by David Phillip Oster on 8/11/13.
//  Copyright (c) 2013 David Phillip Oster. All rights reserved.
//

#import "AppDelegate.h"
#import "HIDxController.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  [_controller refresh];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
  return YES;
}

- (IBAction)rebuild:(id)sender {
  [_controller refresh];
}

@end
