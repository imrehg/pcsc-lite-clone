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
#include <usbserial.h>

 #define USBDEBUG	1 
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
    
static mach_port_t 		masterPort;
static int                      iInitialized = 0;
typedef struct _intFace {
  IOUSBDeviceInterface  **iface;
  UInt32 usbAddr;
} intrFace, *pIntrFace;

static pIntrFace intFace[USBMAX_READERS];

RESPONSECODE OpenUSB( DWORD lun )
{
    CFDictionaryRef 			USBMatch = 0;
    io_iterator_t 			iter = 0;
    io_service_t 			USBDevice = 0;
    io_service_t        		USBIface = 0;
    kern_return_t			kr;
    io_iterator_t       		iterB = 0;
    SInt32				score;
    IOUSBFindInterfaceRequest 		findInterface;
    IOCFPlugInInterface 		**iodev=NULL;
    IOCFPlugInInterface			**iodevB=NULL;
    IOUSBDeviceInterface 		**dev=NULL;
    HRESULT 				res;
    DWORD				rdrLun;
    UInt8				configNum;
    UInt16				vendor;
    UInt16				product;
    UInt16				release;
    UInt32   				usbAddr;
    short				iFound;
    int					i;
    int					sleepCount;
       
    iFound = 0;
    rdrLun = lun >> 16;
    
    if ( iInitialized == 0 ) {
                
        for (i=0; i < USBMAX_READERS; i++) {
            intFace[i] = (pIntrFace)malloc(sizeof(intrFace));
            (intFace[i])->usbAddr = 0;
        }
    
        /* first create a master_port for my task */
        kr = IOMasterPort(bootstrap_port, &masterPort);
        if (kr || !masterPort)
        {
            printf("ERR: Couldn't create a master IOKit Port(%08x)\n", kr);
            return STATUS_UNSUCCESSFUL;
        }

        iInitialized = 1;
    }

    USBMatch = IOServiceMatching(kIOUSBDeviceClassName);
    if (!USBMatch)
    {
        printf("ERR: Can't create a USB matching dictionary\n");
        mach_port_deallocate(mach_task_self(), masterPort);
        return STATUS_UNSUCCESSFUL;
    }
    
    /* create an iterator over all matching IOService nubs */
    kr = IOServiceGetMatchingServices(masterPort, USBMatch, &iter);
    if (kr)
    {
        printf("ERR: Can't create a USB Service iterator(%08x)\n", kr);
        CFRelease(USBMatch);
        mach_port_deallocate(mach_task_self(), masterPort);
        return STATUS_UNSUCCESSFUL;
    }

    while ( (USBDevice = IOIteratorNext(iter)) )
    {
        
        kr = IOCreatePlugInInterfaceForService(USBDevice, kIOUSBDeviceUserClientTypeID, 							kIOCFPlugInInterfaceID, &iodev, &score);
        IOObjectRelease(USBDevice);	/* done with the device object now */
        if (kr || !iodev)
        {
            printf("ERR: unable to create a plugin (%08x)\n", kr);
            continue;
        }
            
        /* i have the device plugin. I need the device interface */
        res = (*iodev)->QueryInterface(iodev, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), 						(LPVOID)&dev);
        (*iodev)->Release(iodev);
        if (res || !dev)
        {
            printf("ERR: Couldn't create a device interface (%08x)\n", (int)res);
            continue;
        }

        kr = (*dev)->GetDeviceVendor(dev, &vendor);
        kr = (*dev)->GetDeviceProduct(dev, &product);
        kr = (*dev)->GetDeviceReleaseNumber(dev, &release);
        kr = (*dev)->GetLocationID(dev, &usbAddr);

        if ((vendor != kMyVendorID) || (product != kMyProductID))
        {
            /* Continue searching for devices */
            (*dev)->Release(dev);
            dev = 0;
            continue;
        } else {
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
   }


    if (iter) {
        IOObjectRelease(iter);
        iter = 0;
    }
     
    if ( dev == 0 ) {
       return STATUS_UNSUCCESSFUL;  
    }

    for (sleepCount=5; sleepCount>0; sleepCount--) {
      kr = (*dev)->USBDeviceOpen(dev);
    
      if ( kr == kIOReturnExclusiveAccess ) {
        sleep(1);
        continue;
      } else {
        break;
      }
    }
      
    if (kr) {
      printf("ERR: unable to open device: %08x\n", kr);
      (*dev)->Release(dev);
    }
      
    /* Reset in case device is in a bad state    */
    /* Reset is asynchronous and will be removed */
    /* (*dev)->ResetDevice(dev);                 */
        
    (*dev)->GetConfiguration(dev, &configNum); 
    kr = (*dev)->SetConfiguration(dev, configNum);         

    printf("set config is %x\n", kr);    
        
        
    (intFace[rdrLun])->iface   = dev;
    (intFace[rdrLun])->usbAddr = usbAddr;
    
  return STATUS_SUCCESS;  
}

RESPONSECODE WriteUSB( DWORD lun, DWORD length, unsigned char *buffer )
{
    IOReturn		iorv;
    DWORD		rdrLun;
    IOUSBDevRequest     controlRequest;
    
#if USBDEBUG
    int			i;
#endif
     
     rdrLun = lun >> 16;
         
#if USBDEBUG
        printf("Attempt to write: ");
        for ( i = 0; i < length; i++ ) {
            printf("%x ", buffer[i]);
        }
        printf("\n");
#endif

    controlRequest.bmRequestType = USBmakebmRequestType(kUSBOut, kUSBVendor, kUSBDevice);
    controlRequest.bRequest      = buffer[0];
    controlRequest.wValue        = buffer[1]*0x100 + buffer[2];
    controlRequest.wIndex        = buffer[3]*0x100 + buffer[4];
    controlRequest.wLength       = length - 5;
    controlRequest.pData         = (void *)&buffer[5];
    controlRequest.wLenDone      = 0;
        
    /* Write the data */
    iorv = (*(intFace[rdrLun])->iface)->DeviceRequest((intFace[rdrLun])->iface, &controlRequest);
    
    if ( iorv != kIOReturnSuccess ) {
        printf("iokit error is %x\n", iokit_usb_err(79));
        printf("iorv unsuccessful with %d - %x\n", iorv, iorv);
        return STATUS_UNSUCCESSFUL;
    }
            
    return STATUS_SUCCESS;
}

RESPONSECODE ReadUSB( DWORD lun, DWORD *length, unsigned char *buffer )
{

    IOReturn		iorv;
    DWORD		rdrLun;
    IOUSBDevRequest     controlRequest;
    
#if USBDEBUG
    int			i;
#endif
     
     rdrLun = lun >> 16;
         
#if USBDEBUG
        printf("Attempt to read %ld bytes\n", *length);
#endif

    controlRequest.bmRequestType = USBmakebmRequestType(kUSBIn, kUSBVendor, kUSBDevice);
    controlRequest.bRequest      = buffer[0];
    controlRequest.wValue        = buffer[1]*0x100 + buffer[2];
    controlRequest.wIndex        = buffer[3]*0x100 + buffer[4];
    controlRequest.wLength       = *length - 5;
    controlRequest.pData         = (void *)&buffer[5];
    controlRequest.wLenDone      = 0;
        
    /* Write the data */
    iorv = (*(intFace[rdrLun])->iface)->DeviceRequest((intFace[rdrLun])->iface, &controlRequest);
    
#if USBDEBUG
        printf("recvLen %ld\n", controlRequest.wLenDone);
        for ( i = 0; i < controlRequest.wLenDone; i++ ) {
            printf("%x ", buffer[i]);
        }
        printf("\n");
#endif
    

    if ( iorv != kIOReturnSuccess ) {
        printf("iorv unsuccessful with %d - %x\n", iorv, iorv);
        return STATUS_UNSUCCESSFUL;
    }
            
    *length = controlRequest.wLenDone;
    return STATUS_SUCCESS;
}

RESPONSECODE CloseUSB( DWORD lun )
{
    IOReturn iorv;
    DWORD rdrLun;
    
    rdrLun = lun >> 16;

    return STATUS_SUCCESS;
}
 
