/***********************************************************
/       Title: usbserial.c
/       Author: David Corcoran
/      Purpose: Abstracts usb API to serial like calls
************************************************************/

#include <stdio.h>
#include <pcscdefines.h>
#include <usbserial.h>
#include <stdlib.h>
#include <usblinux.h>

/* #define USBDEBUG	1 */
#define USBWRITE_PIPE   0x06
#define USBREAD_PIPE    0x85
#define USBMAX_READERS  4

/* Change the following to uniquely match your reader. */
enum {
    kMyVendorID			= 0x0973,   /* change to match your reader */
    kMyProductID		= 0x0001,   /* change to match your reader */
};

static int usbDevice = 0;
 
#define USB_TYPE_STANDARD               (0x00 << 5)
#define USB_TYPE_CLASS                  (0x01 << 5)
#define USB_TYPE_VENDOR                 (0x02 << 5)
#define USB_TYPE_RESERVED               (0x03 << 5)
 
#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02
#define USB_RECIP_OTHER                 0x03
 
#define USB_DIR_OUT                     0
#define USB_DIR_IN                      0x80

RESPONSECODE OpenUSB( DWORD lun )
{
  int rv;

  usbDevice = open_linux_usb_dev( kMyVendorID, kMyProductID, lun );

  if ( usbDevice <= 0 ) {
    return STATUS_UNSUCCESSFUL;
  }

  return STATUS_SUCCESS;  
}

RESPONSECODE WriteUSB( DWORD lun, DWORD length, unsigned char *buffer )
{
  int rv, len;
  int i, requesttype;
  
#ifdef USBDEBUG
  printf("-> ");
  for (i=0; i < length; i++ ) {
    printf("%x ", buffer[i]);
  } printf("\n");
#endif  

  len         = length - 5;
  requesttype = USB_RECIP_DEVICE|USB_TYPE_VENDOR|USB_DIR_OUT;

  rv = control_linux_usb_dev( usbDevice, requesttype,
			      buffer[0], buffer[1]*0x100+buffer[2],
			      buffer[3]*0x100 + buffer[4], &buffer[5],
			      &len, 100000 );
  
  if ( rv < 0 ) {
    return STATUS_UNSUCCESSFUL;
  }

  return STATUS_SUCCESS;
}

RESPONSECODE ReadUSB( DWORD lun, DWORD *length, unsigned char *buffer )
{
  int rv, len, i;
  int requesttype;

  len = *length-5;

  requesttype = USB_RECIP_DEVICE|USB_TYPE_VENDOR|USB_DIR_IN;

  rv = control_linux_usb_dev( usbDevice, requesttype,
			      buffer[0], buffer[1]*0x100+buffer[2],
			      buffer[3]*0x100 + buffer[4], &buffer[5],
			      &len, 100000 );

  if ( rv < 0 ) {
    return STATUS_UNSUCCESSFUL;
  }

  *length = len;

#ifdef USBDEBUG
  printf("<- ");
  for (i=0; i < len; i++ ) {
    printf("%x ", buffer[i]);
  } printf("\n");
#endif  

  return STATUS_SUCCESS;
}

RESPONSECODE CloseUSB( DWORD lun )
{
    close_linux_usb_dev( usbDevice );
    return STATUS_SUCCESS;
}
