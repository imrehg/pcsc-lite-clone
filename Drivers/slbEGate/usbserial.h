/*       Title: usbserial.c
/       Author: David Corcoran
/      Purpose: Abstracts usb API to serial like calls
*/

RESPONSECODE OpenUSB( DWORD lun );
RESPONSECODE WriteUSB( DWORD lun, DWORD length, unsigned char *Buffer );
RESPONSECODE ReadUSB( DWORD lun, unsigned long *length, unsigned char *Buffer );
RESPONSECODE CloseUSB( DWORD lun );
