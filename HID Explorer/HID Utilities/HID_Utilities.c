//     File: HID_Utilities.c
// Abstract: Implementation of the HID utilities
//  Version: 2.0
// 
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
// Inc. ("Apple") in consideration of your agreement to the following
// terms, and your use, installation, modification or redistribution of
// this Apple software constitutes acceptance of these terms.  If you do
// not agree with these terms, please do not use, install, modify or
// redistribute this Apple software.
// 
// In consideration of your agreement to abide by the following terms, and
// subject to these terms, Apple grants you a personal, non-exclusive
// license, under Apple's copyrights in this original Apple software (the
// "Apple Software"), to use, reproduce, modify and redistribute the Apple
// Software, with or without modifications, in source and/or binary forms;
// provided that if you redistribute the Apple Software in its entirety and
// without modifications, you must retain this notice and the following
// text and disclaimers in all such redistributions of the Apple Software.
// Neither the name, trademarks, service marks or logos of Apple Inc. may
// be used to endorse or promote products derived from the Apple Software
// without specific prior written permission from Apple.  Except as
// expressly stated in this notice, no other rights or licenses, express or
// implied, are granted by Apple herein, including but not limited to any
// patent rights that may be infringed by your derivative works or by other
// works in which the Apple Software may be incorporated.
// 
// The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
// MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
// THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
// OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
// 
// IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
// MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
// AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
// STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// 
// Copyright (C) 2013 Apple Inc. All Rights Reserved.
// 
// ***************************************************
#pragma mark - includes & imports
// -----------------------------------------------------

#include "HID_Utilities_External.h"

// ***************************************************
#pragma mark - typedefs, enums, defines, etc.
// -----------------------------------------------------

// ***************************************************
#pragma mark - local (static) function prototypes
// -----------------------------------------------------

//static void CFSetApplierFunctionCopyToCFArray(const void * value, void * context);
//static CFComparisonResult CFDeviceArrayComparatorFunction(const void * val1, const void * val2, void * context);
//static CFMutableDictionaryRef hu_SetUpMatchingDictionary(UInt32 inUsagePage, UInt32 inUsage);

// ***************************************************
#pragma mark - exported globals
// -----------------------------------------------------

// ***************************************************
#pragma mark - local (static) globals
// -----------------------------------------------------

// ***************************************************
#pragma mark - exported function implementations
// -----------------------------------------------------

// utility routine to dump device info
void HIDDumpDeviceInfo(IOHIDDeviceRef inIOHIDDeviceRef) {
	char cstring[256];
	
	printf("Device: %p = { ", inIOHIDDeviceRef);
	
	char        manufacturer[256] = ""; // name of manufacturer
	CFStringRef tCFStringRef      = IOHIDDevice_GetManufacturer(inIOHIDDeviceRef);
	if (tCFStringRef) {
		(void) CFStringGetCString(tCFStringRef, manufacturer, sizeof(manufacturer), kCFStringEncodingUTF8);
	}
	
	char product[256] = "";      // name of product
	tCFStringRef = IOHIDDevice_GetProduct(inIOHIDDeviceRef);
	if (tCFStringRef) {
		(void) CFStringGetCString(tCFStringRef, product, sizeof(product), kCFStringEncodingUTF8);
	}
	
	printf("%s - %s, ", manufacturer, product);
	
	long vendorID = IOHIDDevice_GetVendorID(inIOHIDDeviceRef);
	if (vendorID) {
		if ( HIDGetVendorNameFromVendorID(vendorID, cstring) ) {
			printf(" vendorID: 0x%04lX (\"%s\"), ", vendorID, cstring);
		} else {
			printf(" vendorID: 0x%04lX, ",          vendorID);
		}
	}
	
	long productID = IOHIDDevice_GetProductID(inIOHIDDeviceRef);
	if (productID) {
		if ( HIDGetProductNameFromVendorProductID(vendorID, productID, cstring) ) {
			printf(" productID: 0x%04lX (\"%s\"), ", productID, cstring);
		} else {
			printf(" productID: 0x%04lX, ",          productID);
		}
	}
	
	uint32_t usagePage = IOHIDDevice_GetUsagePage(inIOHIDDeviceRef);
	uint32_t usage     = IOHIDDevice_GetUsage(inIOHIDDeviceRef);
	if (!usagePage || !usage) {
		usagePage = IOHIDDevice_GetPrimaryUsagePage(inIOHIDDeviceRef);
		usage     = IOHIDDevice_GetPrimaryUsage(inIOHIDDeviceRef);
	}
	
	printf("usage: 0x%04lX:0x%04lX, ", (long unsigned int) usagePage, (long unsigned int) usage);
	
	tCFStringRef = HIDCopyUsageName(usagePage, usage);
	if (tCFStringRef) {
		(void) CFStringGetCString(tCFStringRef, cstring, sizeof(cstring), kCFStringEncodingUTF8);
		printf("\"%s\", ", cstring);
		CFRelease(tCFStringRef);
	}

	tCFStringRef = IOHIDDevice_GetTransport(inIOHIDDeviceRef);
	if (tCFStringRef) {
		(void) CFStringGetCString(tCFStringRef, cstring, sizeof(cstring), kCFStringEncodingUTF8);
		printf("Transport: \"%s\", ", cstring);
	}
	
	long vendorIDSource = IOHIDDevice_GetVendorIDSource(inIOHIDDeviceRef);
	if (vendorIDSource) {
		printf("VendorIDSource: %ld, ", vendorIDSource);
	}
	
	long version = IOHIDDevice_GetVersionNumber(inIOHIDDeviceRef);
	if (version) {
		printf("version: %ld, ", version);
	}
	
	tCFStringRef = IOHIDDevice_GetSerialNumber(inIOHIDDeviceRef);
	if (tCFStringRef) {
		(void) CFStringGetCString(tCFStringRef, cstring, sizeof(cstring), kCFStringEncodingUTF8);
		printf("SerialNumber: \"%s\", ", cstring);
	}
	
	long country = IOHIDDevice_GetCountryCode(inIOHIDDeviceRef);
	if (country) {
		printf("CountryCode: %ld, ", country);
	}
	
	long locationID = IOHIDDevice_GetLocationID(inIOHIDDeviceRef);
	if (locationID) {
		printf("locationID: 0x%08lX, ", locationID);
	}
	
#if false
	CFArrayRef pairs = IOHIDDevice_GetUsagePairs(inIOHIDDeviceRef);
	if (pairs) {
		CFIndex idx, cnt = CFArrayGetCount(pairs);
		for (idx = 0; idx < cnt; idx++) {
			const void * pair = CFArrayGetValueAtIndex(pairs, idx);
			CFShow(pair);
		}
	}
#endif // if false

	long maxInputReportSize = IOHIDDevice_GetMaxInputReportSize(inIOHIDDeviceRef);
	if (maxInputReportSize) {
		printf("MaxInputReportSize: %ld, ", maxInputReportSize);
	}
	
	long maxOutputReportSize = IOHIDDevice_GetMaxOutputReportSize(inIOHIDDeviceRef);
	if (maxOutputReportSize) {
		printf("MaxOutputReportSize: %ld, ", maxOutputReportSize);
	}
	
	long maxFeatureReportSize = IOHIDDevice_GetMaxFeatureReportSize(inIOHIDDeviceRef);
	if (maxFeatureReportSize) {
		printf("MaxFeatureReportSize: %ld, ", maxOutputReportSize);
	}
	
	long reportInterval = IOHIDDevice_GetReportInterval(inIOHIDDeviceRef);
	if (reportInterval) {
		printf("ReportInterval: %ld, ", reportInterval);
	}
	
	IOHIDQueueRef queueRef = IOHIDDevice_GetQueue(inIOHIDDeviceRef);
	if (queueRef) {
		printf("queue: %p, ", queueRef);
	}
	
	IOHIDTransactionRef transactionRef = IOHIDDevice_GetTransaction(inIOHIDDeviceRef);
	if (transactionRef) {
		printf("transaction: %p, ", transactionRef);
	}
	
	printf("}\n");
	fflush(stdout);
}   // HIDDumpDeviceInfo

// utility routine to dump element info
void HIDDumpElementInfo(IOHIDElementRef inIOHIDElementRef) {
	if (inIOHIDElementRef) {
		printf("    Element: %p = { ", inIOHIDElementRef);
#if false
		IOHIDDeviceRef tIOHIDDeviceRef = IOHIDElementGetDevice(inIOHIDElementRef);
		printf("Device: %p, ", tIOHIDDeviceRef);
#endif // if 0
		IOHIDElementRef parentIOHIDElementRef = IOHIDElementGetParent(inIOHIDElementRef);
		printf("parent: %p, ", parentIOHIDElementRef);
#if false
		CFArrayRef childrenCFArrayRef = IOHIDElementGetChildren(inIOHIDElementRef);
		printf("children: %p: { ", childrenCFArrayRef);
		fflush(stdout);
		CFShow(childrenCFArrayRef);
		fflush(stdout);
		printf(" }, ");
#endif // if 0
		IOHIDElementCookie tIOHIDElementCookie = IOHIDElementGetCookie(inIOHIDElementRef);
		printf("cookie: 0x%08lX, ", (long unsigned int) tIOHIDElementCookie);
		
		IOHIDElementType tIOHIDElementType = IOHIDElementGetType(inIOHIDElementRef);
		
		switch (tIOHIDElementType) {
			case kIOHIDElementTypeInput_Misc:
			{
				printf("type: Misc, ");
				break;
			}
				
			case kIOHIDElementTypeInput_Button:
			{
				printf("type: Button, ");
				break;
			}
				
			case kIOHIDElementTypeInput_Axis:
			{
				printf("type: Axis, ");
				break;
			}
				
			case kIOHIDElementTypeInput_ScanCodes:
			{
				printf("type: ScanCodes, ");
				break;
			}
				
			case kIOHIDElementTypeOutput:
			{
				printf("type: Output, ");
				break;
			}
				
			case kIOHIDElementTypeFeature:
			{
				printf("type: Feature, ");
				break;
			}
				
			case kIOHIDElementTypeCollection:
			{
				IOHIDElementCollectionType tIOHIDElementCollectionType = IOHIDElementGetCollectionType(inIOHIDElementRef);
				
				switch (tIOHIDElementCollectionType) {
					case kIOHIDElementCollectionTypePhysical:
					{
						printf("type: Physical Collection, ");
						break;
					}
						
					case kIOHIDElementCollectionTypeApplication:
					{
						printf("type: Application Collection, ");
						break;
					}
						
					case kIOHIDElementCollectionTypeLogical:
					{
						printf("type: Logical Collection, ");
						break;
					}
						
					case kIOHIDElementCollectionTypeReport:
					{
						printf("type: Report Collection, ");
						break;
					}
						
					case kIOHIDElementCollectionTypeNamedArray:
					{
						printf("type: Named Array Collection, ");
						break;
					}
						
					case kIOHIDElementCollectionTypeUsageSwitch:
					{
						printf("type: Usage Switch Collection, ");
						break;
					}
						
					case kIOHIDElementCollectionTypeUsageModifier:
					{
						printf("type: Usage Modifier Collection, ");
						break;
					}
						
					default:
					{
						printf("type: %p Collection, ", (void *) tIOHIDElementCollectionType);
						break;
					}
				} // switch
				
				break;
			}
				
			default:
			{
				printf("type: %p, ", (void *) tIOHIDElementType);
				break;
			}
		}     /* switch */
		
		uint32_t usagePage = IOHIDElementGetUsagePage(inIOHIDElementRef);
		uint32_t usage     = IOHIDElementGetUsage(inIOHIDElementRef);
		printf("usage: 0x%04lX:0x%04lX, ", (long unsigned int) usagePage, (long unsigned int) usage);

		CFStringRef tCFStringRef = HIDCopyUsageName(usagePage, usage);
		if (tCFStringRef) {
			char usageString[256] = "";
			(void) CFStringGetCString(tCFStringRef, usageString, sizeof(usageString), kCFStringEncodingUTF8);
			printf("\"%s\", ", usageString);
			CFRelease(tCFStringRef);
		}

		CFStringRef nameCFStringRef = IOHIDElementGetName(inIOHIDElementRef);
		char        buffer[256];
		if ( nameCFStringRef && CFStringGetCString(nameCFStringRef, buffer, sizeof(buffer), kCFStringEncodingUTF8) ) {
			printf("name: %s, ", buffer);
		}
		
		uint32_t reportID    = IOHIDElementGetReportID(inIOHIDElementRef);
		uint32_t reportSize  = IOHIDElementGetReportSize(inIOHIDElementRef);
		uint32_t reportCount = IOHIDElementGetReportCount(inIOHIDElementRef);
		printf("report: { ID: %lu, Size: %lu, Count: %lu }, ",
		       (long unsigned int) reportID, (long unsigned int) reportSize, (long unsigned int) reportCount);
		
		uint32_t unit    = IOHIDElementGetUnit(inIOHIDElementRef);
		uint32_t unitExp = IOHIDElementGetUnitExponent(inIOHIDElementRef);
		if (unit || unitExp) {
			printf("unit: %lu * 10^%lu, ", (long unsigned int) unit, (long unsigned int) unitExp);
		}
		
		CFIndex logicalMin = IOHIDElementGetLogicalMin(inIOHIDElementRef);
		CFIndex logicalMax = IOHIDElementGetLogicalMax(inIOHIDElementRef);
		if (logicalMin != logicalMax) {
			printf("logical: {min: %ld, max: %ld}, ", logicalMin, logicalMax);
		}
		
		CFIndex physicalMin = IOHIDElementGetPhysicalMin(inIOHIDElementRef);
		CFIndex physicalMax = IOHIDElementGetPhysicalMax(inIOHIDElementRef);
		if (physicalMin != physicalMax) {
			printf("physical: {min: %ld, max: %ld}, ", physicalMin, physicalMax);
		}
		
		Boolean isVirtual = IOHIDElementIsVirtual(inIOHIDElementRef);
		if (isVirtual) {
			printf("isVirtual, ");
		}
		
		Boolean isRelative = IOHIDElementIsRelative(inIOHIDElementRef);
		if (isRelative) {
			printf("isRelative, ");
		}
		
		Boolean isWrapping = IOHIDElementIsWrapping(inIOHIDElementRef);
		if (isWrapping) {
			printf("isWrapping, ");
		}
		
		Boolean isArray = IOHIDElementIsArray(inIOHIDElementRef);
		if (isArray) {
			printf("isArray, ");
		}
		
		Boolean isNonLinear = IOHIDElementIsNonLinear(inIOHIDElementRef);
		if (isNonLinear) {
			printf("isNonLinear, ");
		}
		
		Boolean hasPreferredState = IOHIDElementHasPreferredState(inIOHIDElementRef);
		if (hasPreferredState) {
			printf("hasPreferredState, ");
		}
		
		Boolean hasNullState = IOHIDElementHasNullState(inIOHIDElementRef);
		if (hasNullState) {
			printf("hasNullState, ");
		}
		
		printf(" }\n");
	}
}   // HIDDumpElementInfo

void HIDDumpElementCalibrationInfo(IOHIDElementRef inIOHIDElementRef) {
	printf("    Element: %p = { ", inIOHIDElementRef);
	
	CFIndex calMin = IOHIDElement_GetCalibrationMin(inIOHIDElementRef);
	CFIndex calMax = IOHIDElement_GetCalibrationMax(inIOHIDElementRef);
	printf("cal: {min: %ld, max: %ld}, ", calMin, calMax);
	
	CFIndex satMin = IOHIDElement_GetCalibrationSaturationMin(inIOHIDElementRef);
	CFIndex satMax = IOHIDElement_GetCalibrationSaturationMax(inIOHIDElementRef);
	printf("sat: {min: %ld, max: %ld}, ", satMin, satMax);
	
	CFIndex deadMin = IOHIDElement_GetCalibrationDeadZoneMin(inIOHIDElementRef);
	CFIndex deadMax = IOHIDElement_GetCalibrationDeadZoneMax(inIOHIDElementRef);
	printf("dead: {min: %ld, max: %ld}, ", deadMin, deadMax);
	
	double_t granularity = IOHIDElement_GetCalibrationGranularity(inIOHIDElementRef);
	printf("granularity: %6.2f }\n", granularity);
} // HIDDumpElementCalibrationInfo

// ***************************************************
#pragma mark - local (static) function implementations
// -----------------------------------------------------

// *************************************************************************
//
// CFSetApplierFunctionCopyToCFArray(value, context)
//
// Purpose:	CFSetApplierFunction to copy the CFSet to a CFArray
//
// Notes:	called one time for each item in the CFSet
//
// Inputs:	value           - the current element of the CFSet
// context			- the CFMutableArrayRef we're adding the CFSet elements to
//
// Returns:	nothing
//
//static void CFSetApplierFunctionCopyToCFArray(const void * value, void * context) {
//	// printf("%s: 0x%08lX\n", __PRETTY_FUNCTION__, (long unsigned int) value);
//	CFArrayAppendValue( (CFMutableArrayRef) context, value );
//}   // CFSetApplierFunctionCopyToCFArray

// ---------------------------------
// used to sort the CFDevice array after copying it from the (unordered) (CF)set.
// we compare based on the location ID's since they're consistant (across boots & launches).
//
//static CFComparisonResult CFDeviceArrayComparatorFunction(const void * val1, const void * val2, void * context) {
//#pragma unused(context)
//	CFComparisonResult result = kCFCompareEqualTo;
//
//	long               loc1 = IOHIDDevice_GetLocationID( (IOHIDDeviceRef) val1 );
//	long               loc2 = IOHIDDevice_GetLocationID( (IOHIDDeviceRef) val2 );
//	if (loc1 < loc2) {
//		result = kCFCompareLessThan;
//	} else if (loc1 > loc2) {
//		result = kCFCompareGreaterThan;
//	}
//
//	return (result);
//}   // CFDeviceArrayComparatorFunction

// *************************************************************************
//
// hu_SetUpMatchingDictionary(inUsagePage, inUsage)
//
// Purpose:	builds a matching dictionary based on usage page and usage
//
// Notes:	Only called by HIDBuildMultiDeviceList
//
// Inputs:	inUsagePage				- usage page
// inUsage					- usages
//
// Returns:	CFMutableDictionaryRef  - the matching dictionary
//

//static CFMutableDictionaryRef hu_SetUpMatchingDictionary(UInt32 inUsagePage, UInt32 inUsage) {
//	// create a dictionary to add usage page/usages to
//	CFMutableDictionaryRef refHIDMatchDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault,
//	                                                                         0,
//	                                                                         &kCFTypeDictionaryKeyCallBacks,
//	                                                                         &kCFTypeDictionaryValueCallBacks);
//
//	if (refHIDMatchDictionary) {
//		if (inUsagePage) {
//			// Add key for device type to refine the matching dictionary.
//			CFNumberRef pageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsagePage);
//			if (pageCFNumberRef) {
//				CFDictionarySetValue(refHIDMatchDictionary,
//				                     CFSTR(kIOHIDPrimaryUsagePageKey), pageCFNumberRef);
//				CFRelease(pageCFNumberRef);
//				// note: the usage is only valid if the usage page is also defined
//				if (inUsage) {
//					CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsage);
//					if (usageCFNumberRef) {
//						CFDictionarySetValue(refHIDMatchDictionary,
//						                     CFSTR(kIOHIDPrimaryUsageKey), usageCFNumberRef);
//						CFRelease(usageCFNumberRef);
//					} else {
//						fprintf(stderr, "%s: CFNumberCreate(usage) failed.", __PRETTY_FUNCTION__);
//					}
//				}
//			} else {
//				fprintf(stderr, "%s: CFNumberCreate(usage page) failed.", __PRETTY_FUNCTION__);
//			}
//		}
//	} else {
//		fprintf(stderr, "%s: CFDictionaryCreateMutable failed.", __PRETTY_FUNCTION__);
//	}
//
//	return (refHIDMatchDictionary);
//}   // hu_SetUpMatchingDictionary
