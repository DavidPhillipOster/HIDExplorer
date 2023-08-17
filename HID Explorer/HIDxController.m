//
//  HIDxController.m
//  HID Explorer
//
//  Created by david on 8/11/13.
//  Copyright (c) 2013 David Phillip Oster. All rights reserved.
//

#import "HIDxController.h"
#import <CoreFoundation/CoreFoundation.h>
#import "HID_Utilities_External.h"
#import "HIDxElement.h"

NSString *StringFromElementType(IOHIDElementRef element, IOHIDElementType elementType);

@implementation HIDxController {
	IOHIDManagerRef _tIOHIDManagerRef;
  NSInteger _selectedDeviceIndex;
  NSInteger _selectedElementIndex;
  NSTimer *_timer;
}

- (id)init {
  self = [super init];
  if (self) {
    [self hidxControllerInit];
  }
  return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder{
  self = [super init];
  if (self) {
    [self hidxControllerInit];
  }
  return self;
}

- (void)hidxControllerInit {
  _selectedDeviceIndex = -1;
  _selectedElementIndex = -1;
  __weak typeof(self) weakself = self;
  _timer = [NSTimer scheduledTimerWithTimeInterval:0.25 target:weakself selector:@selector(timerFired:) userInfo:nil repeats:YES];
}

- (void)dealloc {
  [_timer invalidate];
  _timer = nil;
}

- (void)timerFired:(NSTimer *)timer {
    if (0 <= _selectedDeviceIndex && _selectedDeviceIndex < [_selectDevice numberOfItems] &&
        0 <= _selectedElementIndex && _selectedElementIndex < [_selectElement numberOfItems]) {
      [_raw poll];
    }
}

// Reset a device's properties. Select an a device.
- (void)selectDeviceIndex:(NSInteger)index {
  if (0 <= index && index < [_selectDevice numberOfItems]) {
    _selectedDeviceIndex = index;
    IOHIDDeviceRef dev = (__bridge IOHIDDeviceRef)([[_selectDevice itemAtIndex:index] representedObject]);

    // ---- 
    NSString *product = (__bridge NSString *)IOHIDDevice_GetProduct(dev);
    if (nil == product) {
      product = @"";
    }
    [_product setStringValue:product];

    // ----
    NSString *manufacturer = (__bridge NSString *)(IOHIDDevice_GetManufacturer(dev));
    if (nil == manufacturer) {
      manufacturer = @"";
    }
    [_manufacturer setStringValue:manufacturer];

    // ---- 
    char cstring[256];
    long vendorIDn = IOHIDDevice_GetVendorID(dev);
    NSString *vendorID = nil;
    if (vendorIDn) {
      vendorID = [NSString stringWithFormat:@"0x%04lX", vendorIDn];
      if (HIDGetVendorNameFromVendorID(vendorIDn, cstring)) {
        vendorID = [NSString stringWithFormat:@"%@ (%s)", vendorID, cstring];
      }
    }
    if (nil == vendorID) {
      vendorID = @"";
    }
    [_vendorID setStringValue:vendorID];

    // ---- 
    long productIDn = IOHIDDevice_GetProductID(dev);
    NSString *productID = nil;
    if (productIDn) {
      if (HIDGetProductNameFromVendorProductID(vendorIDn, productIDn, cstring)) {
        productID = [NSString stringWithFormat:@"0x%04lX (%s)", productIDn, cstring];
      } else {
        productID = [NSString stringWithFormat:@"0x%04lX", productIDn];
      }
    }
    if (nil == productID) {
      productID = @"";
    }
    [_productID setStringValue:productID];

    // ---- 
    uint32_t usagePage = IOHIDDevice_GetUsagePage(dev);
    uint32_t usageN     = IOHIDDevice_GetUsage(dev);
    NSString *usage = nil;
    if (!usagePage || !usageN) {
      usagePage = IOHIDDevice_GetPrimaryUsagePage(dev);
      usageN     = IOHIDDevice_GetPrimaryUsage(dev);
    }
    usage = [NSString stringWithFormat:@"0x%02lX:0x%02lX", (long unsigned int) usagePage, (long unsigned int) usageN];
    NSString *usageName = CFBridgingRelease(HIDCopyUsageName(usagePage, usageN));
    if (usageName) {
      if ([usageName hasPrefix:@"Consumer Consumer"]) {
        usageName = [usageName stringByReplacingCharactersInRange:NSMakeRange(0, 9) withString:@""];
      }
      usage = [NSString stringWithFormat:@"%@ %@", usage, usageName];
    }
    [_deviceUsage setStringValue:usage];

    // ----
    NSString *transport = (__bridge NSString *)(IOHIDDevice_GetTransport(dev));
    if (nil == transport) {
      transport = @"";
    }
    [_transport setStringValue:transport];

    // ----
    long versionN = IOHIDDevice_GetVersionNumber(dev);
    NSString *version = @"";
    if (versionN) {
      version = [NSString stringWithFormat:@"%ld", versionN];
    }
    [_version setStringValue:version];
	
    // ---- 
    NSString *serialNumber = (__bridge NSString *)(IOHIDDevice_GetSerialNumber(dev));
    if (nil == serialNumber) {
      serialNumber = @"";
    }
    [_serialNumber setStringValue:serialNumber];

    // ---- 
    long locationIDn = IOHIDDevice_GetLocationID(dev);
    NSString *locationID = @"";
    if (locationIDn) {
      locationID = [NSString stringWithFormat:@"0x%08lX", locationIDn];
    }
    [_locationID setStringValue:locationID];

    [_selectElement removeAllItems];
    NSArray *rawElements = CFBridgingRelease(IOHIDDeviceCopyMatchingElements(dev, NULL, 0));
     for (id elementAsID in rawElements) {
      IOHIDElementRef element = (__bridge IOHIDElementRef)(elementAsID);

      NSString *name = [self nameForElement:element prefix:@""];

      NSArray *children = nil;
      children = (__bridge NSArray *)(IOHIDElementGetChildren(element));
      if ([children count]) {
#if 0 // It's useless to recurse into the children. so don't.
        [_selectElement addItemWithTitle:name];
        NSMenuItem *item = [_selectElement lastItem];
        [item setRepresentedObject:elementAsID];

        IOHIDElementCookie cookieN = IOHIDElementGetCookie(element);
        NSString *prefix = [NSString stringWithFormat:@"0x%02lX", (long unsigned int)cookieN];
        NSMenu *subMenu = [[NSMenu alloc] initWithTitle:name];
        [item setSubmenu:subMenu];
        [self recursivelyAddElements:children prefix:prefix to:subMenu];
#endif
      } else {
        [_selectElement addItemWithTitle:name];
        NSMenuItem *item = [_selectElement lastItem];
        [item setRepresentedObject:elementAsID];
      }

      // TODO: build the element menu
      // As with Device, build a hierarchical menu of elements, whose represented object is the element 
    }
  }
}

- (NSString *)nameForElement:(IOHIDElementRef)element prefix:(NSString *)prefix {
  IOHIDElementCookie cookieN = IOHIDElementGetCookie(element);
  IOHIDElementType elementType = IOHIDElementGetType(element);
  NSString *cookie = [NSString stringWithFormat:@"0x%02lX %@", (long unsigned int) cookieN, StringFromElementType(element, elementType)];
  if ([prefix length]) {
    cookie = [NSString stringWithFormat:@"%@ %@", prefix, cookie];
  }
  return cookie;
}

- (void)recursivelyAddElements:(NSArray *)children prefix:(NSString *)prefix to:(NSMenu *)subMenu {
  [subMenu setAutoenablesItems:NO];
  for (id elementAsID in children) {
    IOHIDElementRef element = (__bridge IOHIDElementRef)(elementAsID);
    
    NSString *name = [self nameForElement:element prefix:prefix];
    
    [subMenu addItemWithTitle:name action:@selector(subMenuItemSelected:) keyEquivalent:@""];
    NSMenuItem *item = [subMenu itemAtIndex:[subMenu numberOfItems]-1];
    [item setRepresentedObject:elementAsID];
    
    NSArray *children = (__bridge NSArray *)(IOHIDElementGetChildren(element));
    if ([children count]) {
      IOHIDElementCookie cookieN = IOHIDElementGetCookie(element);
      prefix = [NSString stringWithFormat:@"%@ 0x%02lX", prefix, (long unsigned int)cookieN];
      NSMenu *subMenu1 = [[NSMenu alloc] initWithTitle:name];
      [item setSubmenu:subMenu1];
      [self recursivelyAddElements:children prefix:prefix to:subMenu1];
    }
  }
}


- (IBAction)subMenuItemSelected:(id)sender {
  NSLog(@"%@", sender);
}

- (void)selectElementIndex:(NSInteger)index {
  if (0 <= index && index < [_selectElement numberOfItems]) {
    _selectedElementIndex = index;
//    IOHIDDeviceRef dev = (__bridge IOHIDDeviceRef)([[_selectDevice itemAtIndex:index] representedObject]);
    id elementAsID = [[_selectElement itemAtIndex:index] representedObject];
    IOHIDElementRef element = (__bridge IOHIDElementRef)(elementAsID);
    IOHIDElementCookie cookieN = IOHIDElementGetCookie(element);
    [_cookie setStringValue:[NSString stringWithFormat:@"0x%02lX", (long unsigned int)cookieN]];
    IOHIDElementType elementType = IOHIDElementGetType(element);
    [_raw setElement:element];
    [_calibrated setElement:element];
    [_type setStringValue:StringFromElementType(element, elementType)];
		uint32_t usagePage = IOHIDElementGetUsagePage(element);
		uint32_t usage     = IOHIDElementGetUsage(element);
    [_elementUsage setStringValue:[NSString stringWithFormat:@"0x%04lX:0x%04lX",
        (long unsigned int) usagePage, (long unsigned int) usage]];
    NSString *nameS = (__bridge NSString *)IOHIDElementGetName(element);
    if (nil == nameS) {
      nameS = @"";
    }
    [_name setStringValue:nameS];
    CFIndex scaledMin = IOHIDElementGetPhysicalMin(element);
    CFIndex scaledMax = IOHIDElementGetPhysicalMax(element);
    NSString *scaledMinS = @"";
    NSString *scaledMaxS = @"";
    NSString *scaledRange = @"";
    if (scaledMin || scaledMax) {
      scaledRange = [NSString stringWithFormat:@"%ld .. %ld", scaledMin, scaledMax];
      scaledMinS = [NSString stringWithFormat:@"%ld", scaledMin];
      scaledMaxS = [NSString stringWithFormat:@"%ld", scaledMax];
    }
    [_scaledRange setStringValue:scaledRange];
    [_scaledMin setStringValue:scaledMinS];
    [_scaledMax setStringValue:scaledMaxS];

    CFIndex min = IOHIDElementGetLogicalMin(element);
    CFIndex max = IOHIDElementGetLogicalMax(element);
    NSString *minS = @"";
    NSString *maxS = @"";
    NSString *range = @"";
    if (min || max) {
      range = [NSString stringWithFormat:@"%ld .. %ld", min, max];
      minS = [NSString stringWithFormat:@"%ld", min];
      maxS = [NSString stringWithFormat:@"%ld", max];
    }
    [_raw setIntegerMin:scaledMin];
    [_raw setIntegerMax:scaledMax];
    [_range setStringValue:range];
    [_rawMin setStringValue:minS];
    [_rawMax setStringValue:maxS];
    // size in bits.
    [_size setIntValue:IOHIDElementGetReportSize(element)];

    double calibMin = IOHIDElement_GetCalibrationMin(element);
    NSString *calibMinS = @"";
    double calibMax = IOHIDElement_GetCalibrationMax(element);
    NSString *calibMaxS = @"";
   if (calibMin < calibMax) {
      calibMinS = [NSString stringWithFormat:@"%4.2g", calibMin];
      calibMaxS = [NSString stringWithFormat:@"%4.2g", calibMax];
      [_calibrated setIntegerMin:calibMin];
      [_calibrated setIntegerMax:calibMax];
    } else {
      [_calibrated setIntegerMin:0];
      [_calibrated setIntegerMax:0];
    }
    [_calibratedMin setStringValue:calibMinS];
    [_calibratedMax setStringValue:calibMaxS];

    uint32_t units = IOHIDElementGetUnit(element);
    uint32_t unitExponent = IOHIDElementGetUnitExponent(element);
    NSString *unitS = @"";
    if (units || unitExponent) {
      unitS = [NSString stringWithFormat:@"%lu x 10^%lu",
        (unsigned long)units,
        (unsigned long)unitExponent];
    }
    [_units setStringValue:unitS];

    [_relative setIntValue:IOHIDElementIsRelative(element)];
    [_preferredState setIntValue:IOHIDElementHasPreferredState(element)];
    [_wrapping setIntValue:IOHIDElementIsWrapping(element)];
    [_nonLinear setIntValue:IOHIDElementIsNonLinear(element)];
    [_nullState setIntValue:IOHIDElementHasNullState(element)];
    [_virtualX setIntValue:IOHIDElementIsVirtual(element)];
  } else {
    [_raw setElement:NULL];
    [_calibrated setElement:NULL];
  }
}

- (IBAction)deviceChanged:(NSPopUpButton *)sender {
  [self selectDeviceIndex:[sender indexOfSelectedItem]];
}

- (IBAction)elementChanged:(NSPopUpButton *)sender {
  [self selectElementIndex:[sender indexOfSelectedItem]];
}

// Reset the list of devices. Select a device.
- (void)refresh {
	CFSetRef deviceCFSetRef = NULL;
	IOHIDDeviceRef *tIOHIDDeviceRefs = nil;

  _tIOHIDManagerRef = IOHIDManagerCreate(kCFAllocatorDefault,
                                        kIOHIDOptionsTypeNone);
  if (NULL == _tIOHIDManagerRef) {
    NSLog(@"NULL == _tIOHIDManagerRef");
    return;
  }

  IOHIDManagerSetDeviceMatching(_tIOHIDManagerRef, NULL);

  IOReturn tIOReturn = IOHIDManagerOpen(_tIOHIDManagerRef, kIOHIDOptionsTypeNone);
  if (noErr != tIOReturn) {
    NSLog(@"noErr != tIOReturn: %X", tIOReturn);
    return;
  }

  deviceCFSetRef = IOHIDManagerCopyDevices(_tIOHIDManagerRef);
  if (NULL == deviceCFSetRef) {
    NSLog(@"NULL == deviceCFSetRef");
    return;
  }

  CFIndex deviceCount = CFSetGetCount(deviceCFSetRef);

  tIOHIDDeviceRefs = malloc(sizeof(IOHIDDeviceRef) * deviceCount);
  if (NULL == tIOHIDDeviceRefs) {
		CFRelease(deviceCFSetRef);
    return;
  }
  CFSetGetValues(deviceCFSetRef, (const void **)tIOHIDDeviceRefs);
  CFRelease(deviceCFSetRef);
  deviceCFSetRef = NULL;
  CFIndex deviceIndex;
  NSMutableArray *devices = [NSMutableArray array];
  for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
    if (NULL == tIOHIDDeviceRefs[deviceIndex]) {
      continue;
    }

    // name of device
    CFStringRef tCFStringRef = IOHIDDevice_GetProduct(tIOHIDDeviceRefs[deviceIndex]);
    if (tCFStringRef) {
      [devices addObject:[@{
           @"name": (__bridge NSString *)tCFStringRef,
           @"dev" : (__bridge id)(tIOHIDDeviceRefs[deviceIndex])} mutableCopy]];
    }
  }
  NSComparator comparator = ^(id obj1, id obj2) {
    NSDictionary *dict1 = (NSDictionary *)obj1;
    NSDictionary *dict2 = (NSDictionary *)obj2;
    NSComparisonResult result = [dict1[@"name"] caseInsensitiveCompare:dict2[@"name"]];
    if (result == NSOrderedSame) {
      // TODO: secondary sort key should be usage page.
      NSInteger diff = (NSInteger)dict1[@"dev"] - (NSInteger)dict2[@"dev"];
      if (diff < 0) {
        result = NSOrderedAscending;
      } else if (0 < diff) {
        result = NSOrderedDescending;
      }
    }
    return result;
  };
  [devices sortUsingComparator:comparator];
  [_selectDevice removeAllItems];
  int count = (int)[devices count];
  // Give all devices unique names.
  for (int i = count - 2;0 <= i; i--) {
    if ([devices[i][@"name"] isEqual:devices[i+1][@"name"]]) {
      for(int j = 1;0 <= i && [devices[i][@"name"] isEqual:devices[i+1][@"name"]]; j++, --i) {
        devices[i+1][@"name"] = [NSString stringWithFormat:@"%@ %d", devices[i+1][@"name"], j];
      }
    }
  }
  // Put names in increasing order.
  [devices sortUsingComparator:comparator];
  for (NSDictionary *dict in devices) {
    [_selectDevice addItemWithTitle:dict[@"name"]];
    [[_selectDevice lastItem] setRepresentedObject:dict[@"dev"]];
  }

	if (tIOHIDDeviceRefs) {
		free(tIOHIDDeviceRefs);
	}
	if (deviceCFSetRef) {
		CFRelease(deviceCFSetRef);
	}

  if (0 <= _selectedDeviceIndex && _selectedDeviceIndex < [_selectDevice numberOfItems]) {
    // TODO: should reselect by name after rebuild
    [self selectDeviceIndex:_selectedDeviceIndex];
  } else if ([_selectDevice numberOfItems]) {
    [self selectDeviceIndex:0];
  }
}


@end



NSString *StringFromSimpleElementType(IOHIDElementType elementType) {
  switch (elementType) {
    case kIOHIDElementTypeInput_Misc: return @"Misc";
    case kIOHIDElementTypeInput_Button: return @"Button";
    case kIOHIDElementTypeInput_Axis: return @"Axis";
    case kIOHIDElementTypeInput_ScanCodes: return @"ScanCodes";
    case kIOHIDElementTypeInput_NULL: return @"Nul";
    case kIOHIDElementTypeOutput: return @"Output";
    case kIOHIDElementTypeFeature: return @"Feature";
    default:
      return [NSString stringWithFormat:@"%p", (void *) elementType];
  }
}

NSString *StringFromElementType(IOHIDElementRef element, IOHIDElementType elementType) {
  if (kIOHIDElementTypeCollection == elementType) {
    IOHIDElementCollectionType collectionType = IOHIDElementGetCollectionType(element);
    switch (collectionType) {
    case kIOHIDElementCollectionTypePhysical: return @"Physical Collection";
    case kIOHIDElementCollectionTypeApplication: return @"Application Collection";
    case kIOHIDElementCollectionTypeLogical: return @"Logical Collection";
    case kIOHIDElementCollectionTypeReport: return @"Report Collection";
    case kIOHIDElementCollectionTypeNamedArray: return @"Named Array Collection";
    case kIOHIDElementCollectionTypeUsageSwitch: return @"Usage Switch Collection";
    case kIOHIDElementCollectionTypeUsageModifier: return @"Usage Modifier Collection";
    default:
      return [NSString stringWithFormat:@"%p Collection", (void *) collectionType];
    }
  } else {
    return StringFromSimpleElementType(elementType);
  }
}

