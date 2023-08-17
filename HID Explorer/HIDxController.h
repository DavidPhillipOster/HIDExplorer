//
//  HIDxController.h
//  HID Explorer
//
//  Created by david on 8/11/13.
//  Copyright (c) 2013 David Phillip Oster. All rights reserved.
//

#import <AppKit/AppKit.h>

@class HIDxElement;

@interface HIDxController : NSObject
@property (assign) IBOutlet NSPopUpButton *selectDevice;
@property (assign) IBOutlet NSTextField *transport;
@property (assign) IBOutlet NSTextField *vendorID;
@property (assign) IBOutlet NSTextField *version;
@property (assign) IBOutlet NSTextField *manufacturer;
@property (assign) IBOutlet NSTextField *product;
@property (assign) IBOutlet NSTextField *serialNumber;
@property (assign) IBOutlet NSTextField *deviceUsage;
@property (assign) IBOutlet NSTextField *locationID;
@property (assign) IBOutlet NSTextField *productID;

@property (assign) IBOutlet NSPopUpButton *selectElement;
@property (assign) IBOutlet NSTextField *type;
@property (assign) IBOutlet NSTextField *elementUsage;
@property (assign) IBOutlet NSTextField *cookie;
@property (assign) IBOutlet NSTextField *range;
@property (assign) IBOutlet NSTextField *units;
@property (assign) IBOutlet NSTextField *name;
@property (assign) IBOutlet NSTextField *size;
@property (assign) IBOutlet NSTextField *scaledRange;

@property (assign) IBOutlet NSButton *relative;
@property (assign) IBOutlet NSButton *preferredState;
@property (assign) IBOutlet NSButton *wrapping;
@property (assign) IBOutlet NSButton *nonLinear;
@property (assign) IBOutlet NSButton *nullState;
@property (assign) IBOutlet NSButton *virtualX;
@property (assign) IBOutlet HIDxElement *raw;
@property (assign) IBOutlet NSTextField *rawMin;
@property (assign) IBOutlet NSTextField *rawMax;
@property (assign) IBOutlet HIDxElement *calibrated;
@property (assign) IBOutlet NSTextField *calibratedMin;
@property (assign) IBOutlet NSTextField *calibratedMax;
@property (assign) IBOutlet HIDxElement *scaled;
@property (assign) IBOutlet NSTextField *scaledMin;
@property (assign) IBOutlet NSTextField *scaledMax;

- (IBAction)deviceChanged:(id)sender;
- (IBAction)elementChanged:(id)sender;
- (void)refresh;
@end
