/*****************************************************************
/
/ File   :   AdmHndlr.c
/ Author :   David Corcoran
/ Date   :   October 15, 1999
/ Purpose:   This handles administrative functions like reset/power.
/            See http://www.linuxnet.com for more information.
/ License:   See file LICENSE
/
******************************************************************/

#include "pcscdefines.h"
#include "AdmHndlr.h"
#include "usbserial.h"
#include <time.h>
#include <unistd.h>

DWORD timeOut = 500000;

DWORD Adm_ResetICC( DWORD Lun, PUCHAR Atr, PDWORD AtrLength) {

  ULONG rv;
  UCHAR ucCommand[MAX_BUFFER_SIZE];
  unsigned long int len;

  ucCommand[0] = 0x90; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
  ucCommand[3] = 0x00; ucCommand[4] = 0x00;  ucCommand[5] = 0x00;

  rv = WriteUSB(Lun, 5, ucCommand);

  printf("Write Return  %d\n", rv == STATUS_SUCCESS);

  ucCommand[0] = 0x83; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
  ucCommand[3] = 0x00; ucCommand[4] = 0x00;  ucCommand[5] = 0x00;

  len = 69;

  rv = ReadUSB(Lun, &len, ucCommand);
  printf("Read Return  %d\n", rv == STATUS_SUCCESS);


  if ( rv == STATUS_SUCCESS ) {
    memcpy(Atr, &ucCommand[5], len);
    *AtrLength = len;
    return STATUS_SUCCESS;
  } else {
    *AtrLength = 0;
    return STATUS_UNSUCCESSFUL;
  }

  
}


DWORD Adm_TransmitICC( DWORD Lun, PUCHAR pucTxBuffer, DWORD ulTxLength, 
		       PUCHAR pucRxBuffer, PDWORD pulRxLength ) {

  DWORD rv;
  UCHAR ucCommand[MAX_BUFFER_SIZE];
  DWORD ulRecvLength, ulTimeout;
  int i;

  ulTimeout = 0;

#define USB_MAXPACKET_SIZE 8


  if ( ulTxLength ) {
    ucCommand[0] = 0x82; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
    ucCommand[3] = 0x00; ucCommand[4] = 0x00;

    /* Here we need to do a SendAPDU command */
    if ( ulTxLength == 50000 ) {
      ucCommand[0] = 0x80;
      ulTxLength = 5;
    }

    memcpy(&ucCommand[5], pucTxBuffer, ulTxLength);
    
    WriteUSB( Lun, ulTxLength+5, ucCommand );
  }

    usleep(1000);

  if ( *pulRxLength ) {
    ucCommand[0] = 0x81; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
    ucCommand[3] = 0x00; ucCommand[4] = 0x00;

    ulRecvLength = *pulRxLength + 5;
    ReadUSB( Lun, &ulRecvLength , ucCommand ); 
    memcpy( pucRxBuffer, &ucCommand[5], ulRecvLength );
  }  


  return STATUS_SUCCESS;
}

