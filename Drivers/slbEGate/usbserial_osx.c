/***********************************************************
/       Title: usbserial.c
/       Author: David Corcoran
/      Purpose: Abstracts usb API to serial like calls
************************************************************/

#include <stdio.h>
#include <mach/mach.h>
#include <mach/mach_error.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFPlugin.h>

#include <IOKit/IOCFPlugIn.h>

#include <assert.h>
#include <pcscdefines.h>
#include <usbserial_osx.h>

/* #define USBDEBUG	1 */
#define USBCONTROL_PIPE 0
#define USBWRITE_PIPE   2
#define USBREAD_PIPE    1
#define USBMAX_READERS  4

/* Change the following to uniquely match your reader. */
enum {
    kMyVendorID			= 0x0973,	/* change to match your reader */
    kMyProductID		= 0x0001,	/* change to match your reader */
    kMyDeviceClass		= kIOUSBAnyClass,
    kMyDeviceSubClass		= kIOUSBAnySubClass,
    kMyDeviceProtocol		= kIOUSBAnyProtocol
};
    
static mach_port_t 		drvMasterPort;
static int                      iInitialized = 0;

typedef struct _intFace {
  IOUSBDeviceInterface182     	**dev;
  UInt32 			usbAddr;
} intrFace, *pIntrFace;

static pIntrFace intFace[USBMAX_READERS];

RESPONSECODE OpenUSB( DWORD lun )
{
    CFMutableDictionaryRef 		USBMatch = 0;
    io_iterator_t 			iter = 0;
    io_service_t 			USBDevice = 0;
    kern_return_t			kr;
    SInt32				score;
    IOCFPlugInInterface 		**iodev=NULL;
    HRESULT 				res;
    DWORD				rdrLun;
    long 				hpManu_id, hpProd_id;
    UInt32   				usbAddr;
    short				iFound;
    int					i;
    IOUSBConfigurationDescriptorPtr     confDesc;
          
    iFound = 0;
    rdrLun = lun >> 16;
    
    if ( iInitialized == 0 ) {
                
        for (i=0; i < USBMAX_READERS; i++) {
            intFace[i] = (pIntrFace)malloc(sizeof(intrFace));
            (intFace[i])->usbAddr = 0;
            (intFace[i])->dev = NULL;
        }
    
        iInitialized = 1;
    }

    /* first create a master_port for my task */
    kr = IOMasterPort(MACH_PORT_NULL, &drvMasterPort);
    if (kr || !drvMasterPort)
    {
            printf("ERR: Couldn't create a master IOKit Port(%08x)\n", kr);
            return STATUS_UNSUCCESSFUL;
    }

    USBMatch = IOServiceMatching(kIOUSBDeviceClassName);
    if (!USBMatch)
    {
        printf("ERR: Can't create a USB matching dictionary\n");
        mach_port_deallocate(mach_task_self(), drvMasterPort);
        return STATUS_UNSUCCESSFUL;
    }
    
    hpManu_id = kMyVendorID;
    hpProd_id = kMyProductID;
    
    CFDictionarySetValue(USBMatch,
                        CFSTR(kUSBVendorName),
                        CFNumberCreate(kCFAllocatorDefault, 
                        kCFNumberSInt32Type,
                        &hpManu_id));
    CFDictionarySetValue(USBMatch,
                        CFSTR(kUSBProductName), 
                        CFNumberCreate(kCFAllocatorDefault,
                        kCFNumberSInt32Type, 
                        &hpProd_id));
    
    /* create an iterator over all matching IOService nubs */
    kr = IOServiceGetMatchingServices(drvMasterPort, USBMatch, &iter);

    if (kr)
    {
        printf("ERR: Can't create a USB Service iterator(%08x)\n", kr);
        CFRelease(USBMatch);
        mach_port_deallocate(mach_task_self(), drvMasterPort);
        return STATUS_UNSUCCESSFUL;
    }

    while ( (USBDevice = IOIteratorNext(iter)) )
    {          
        kr = IOCreatePlugInInterfaceForService(USBDevice, kIOUSBDeviceUserClientTypeID, 							               kIOCFPlugInInterfaceID, &iodev, &score);
        IOObjectRelease(USBDevice);	      /* done with the device object now */
        if (kr || !iodev)
        {
            printf("ERR: unable to create a plugin (%08x)\n", kr);
            continue;
        }
            
        /* i have the device plugin. I need the device interface */
        res = (*iodev)->QueryInterface(iodev, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), 						              (LPVOID)&(intFace[rdrLun])->dev);
        //(*iodev)->Release(iodev);

        if (res || !(intFace[rdrLun])->dev)
        {
            printf("ERR: Couldn't create a device interface (%08x)\n", (int)res);
            continue;
        }
        
        kr = (*(intFace[rdrLun])->dev)->GetLocationID(((intFace[rdrLun])->dev), &usbAddr);

        for (i=0; i < USBMAX_READERS; i++) {
                if ( usbAddr == (intFace[i])->usbAddr ) {
                    iFound = 1;
                    break;
                }
        }
            
        if ( iFound ) {
                iFound = 0;
                continue;
        }
        
            /* We found it now lets break out of the loop */
        break;
   }
   
   if ((intFace[rdrLun])->dev == NULL) 
   {
        /* Device not found */
        CFRelease(USBMatch);
        mach_port_deallocate(mach_task_self(), drvMasterPort);
        return STATUS_UNSUCCESSFUL;
   }


    kr = (*(intFace[rdrLun])->dev)->USBDeviceOpen(((intFace[rdrLun])->dev));
    if (kr == kIOReturnExclusiveAccess) 
    {
        sleep(1);
        // Try to re-open in case Classic is using the device
        if( kr = (*(intFace[rdrLun])->dev)->USBDeviceOpenSeize(((intFace[rdrLun])->dev)) )
        {
            printf("unable to open device: 0x%08X\n", kr);
            (*(intFace[rdrLun])->dev)->Release(((intFace[rdrLun])->dev));
            return STATUS_UNSUCCESSFUL;
        }
    }
    else
    {
        if ( kr != kIOReturnSuccess)
        {
            printf("unable to open device, not kIOReturnExclusiveAccess: 0x%08X\n", kr);
            (*(intFace[rdrLun])->dev)->Release(((intFace[rdrLun])->dev));
            return STATUS_UNSUCCESSFUL;

        }
    }

    kr = (*(intFace[rdrLun])->dev)->GetConfigurationDescriptorPtr(((intFace[rdrLun])->dev), 0, &confDesc); 

    if (kr)
    {
        printf("ERR: unable to get the configuration\n");
        (*(intFace[rdrLun])->dev)->USBDeviceClose(((intFace[rdrLun])->dev));
        (*(intFace[rdrLun])->dev)->Release(((intFace[rdrLun])->dev));
        return STATUS_UNSUCCESSFUL;
    }
    
    (*(intFace[rdrLun])->dev)->ResetDevice(((intFace[rdrLun])->dev));
  
    kr = (*(intFace[rdrLun])->dev)->SetConfiguration(((intFace[rdrLun])->dev),
						     confDesc->bConfigurationValue);
    
    if (kr)
    {
        printf("ERR: unable to set the configuration\n");
        (*(intFace[rdrLun])->dev)->USBDeviceClose(((intFace[rdrLun])->dev));
        (*(intFace[rdrLun])->dev)->Release(((intFace[rdrLun])->dev));
        return STATUS_UNSUCCESSFUL;
    }

    if (iter) {
        IOObjectRelease(iter);
        iter = 0;
    }

  return STATUS_SUCCESS;  
 
}

RESPONSECODE WriteUSB( DWORD lun, DWORD length, unsigned char *buffer )
{
    return 0;
}

RESPONSECODE ReadUSB( DWORD lun, DWORD *length, unsigned char *buffer )
{
    return 0;
}


RESPONSECODE ControlUSB( DWORD lun, DWORD dataDirection, DWORD txLength,
                         PUCHAR txBuffer, DWORD *rxLength,
                         PUCHAR rxBuffer ) {


    IOReturn		iorv;
    DWORD		rdrLun;
    IOUSBDevRequestTO   controlRequest;
    
#if USBDEBUG
    int			i;
#endif
     
     rdrLun = lun >> 16;
  
  if ( dataDirection == 0 ) {
    controlRequest.bmRequestType = USBmakebmRequestType(kUSBOut, kUSBVendor, 								kUSBDevice);
    controlRequest.wLength       = txLength - 5;
    controlRequest.pData         = (void *)&txBuffer[5];

  } else if ( dataDirection == 1 ) {
    controlRequest.bmRequestType = USBmakebmRequestType(kUSBIn, kUSBVendor, 								kUSBDevice);
    controlRequest.wLength       = *rxLength;
    controlRequest.pData         = (void *)rxBuffer;

  }   

  usleep(1000);

#ifdef USBDEBUG
  printf("-> [%d] ", txLength);
  for (i=0; i < txLength; i++ ) {
    printf("%x ", txBuffer[i]);
  } printf("\n");
#endif

    controlRequest.bRequest          = txBuffer[0];
    controlRequest.wValue            = txBuffer[1]*0x100 + txBuffer[2];
    controlRequest.wIndex            = txBuffer[3]*0x100 + txBuffer[4];
    controlRequest.wLenDone          = 0;
    controlRequest.noDataTimeout     = 400000;
    controlRequest.completionTimeout = 400000;
        
    /* Write the data */
    iorv = (*(intFace[rdrLun])->dev)->DeviceRequestTO((intFace[rdrLun])->dev, &controlRequest);
    
  if ( iorv ) {
    return STATUS_UNSUCCESSFUL;
  }

  if ( controlRequest.wLenDone ) {
  
    if (rxLength != 0) {
      *rxLength = controlRequest.wLenDone;
    }                              
#ifdef USBDEBUG  
    printf("<- [%d] ", controlRequest.wLenDone);   
    for (i=0; i < controlRequest.wLenDone; i++ ) {
      printf("%x ", rxBuffer[i]);
    } printf("\n");
#endif
  } else {
    if (rxLength != 0) {
      *rxLength = 0;
    }
  }
   
  return STATUS_SUCCESS;
    
}


RESPONSECODE CloseUSB( DWORD lun )
{
    IOReturn                            iorv;
    DWORD                               rdrLun;
     
    rdrLun = lun >> 16;

    iorv = (*(intFace[rdrLun])->dev)->USBDeviceClose((intFace[rdrLun])->dev);
    if (iorv) {
        printf("ERR: Couldn't close device (%08x)\n", (int)iorv);
    }

    (*(intFace[rdrLun])->dev)->Release((intFace[rdrLun])->dev);
    free (intFace[rdrLun]);

    mach_port_deallocate(mach_task_self(), drvMasterPort);
    if ( iorv != kIOReturnSuccess ) {
        return STATUS_UNSUCCESSFUL;
    }
    
    return STATUS_SUCCESS;
}
 
