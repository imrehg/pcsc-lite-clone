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
#include "usbserial_osx.h"
#include <time.h>
#include <unistd.h>

DWORD timeOut = 500000;

DWORD Adm_ResetICC( DWORD Lun, PUCHAR Atr, PDWORD AtrLength) {

  ULONG rv;
  UCHAR ucCommand[MAX_BUFFER_SIZE];
  UCHAR ucResponse[MAX_BUFFER_SIZE];
  unsigned long int len;

  ucCommand[0] = 0x90; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
  ucCommand[3] = 0x00; ucCommand[4] = 0x00;  ucCommand[5] = 0x00;

  rv = ControlUSB(Lun, 0, 5, ucCommand, 0, ucResponse); 
  //rv = WriteUSB(Lun, 5, ucCommand);

  printf("Write Return  %d\n", rv == STATUS_SUCCESS);

  ucCommand[0] = 0x83; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
  ucCommand[3] = 0x00; ucCommand[4] = 0x00;  ucCommand[5] = 0x00;

  len = 64;

  rv = ControlUSB(Lun, 1, 5, ucCommand, &len, ucResponse);
  //  rv = ReadUSB(Lun, &len, ucCommand);
  printf("Read Return  %d\n", rv == STATUS_SUCCESS);


  if ( rv == STATUS_SUCCESS ) {
    memcpy(Atr, &ucResponse, len);
    *AtrLength = len;
    return STATUS_SUCCESS;
  } else {
    *AtrLength = 0;
    return STATUS_UNSUCCESSFUL;
  }

  
}

UCHAR Adm_PollStatus( DWORD Lun ) {

  DWORD rv;
  UCHAR ucCommand[MAX_BUFFER_SIZE];
  UCHAR ucResponse[MAX_BUFFER_SIZE];
  DWORD ulRecvLength;
  int transferType;

  do {

  ucCommand[0] = 0xA0; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
  ucCommand[3] = 0x00; ucCommand[4] = 0x00;    

  ulRecvLength  = 1;
  ucResponse[0] = 0;
  ControlUSB(Lun, 1, 5, ucCommand, 
	     &ulRecvLength, ucResponse);


  if ( ucResponse[0] & 0x10 ) {
    transferType = 1;
    break;
  } else if ( ucResponse[0] & 0x20 ) {
    transferType = 2;
    break;
  } else if ( ucResponse[0] & 0x40 ) {
    usleep(5000);
    continue;
  } else if ( ucResponse[0] & 0x80 ) {
    return -1;
  } else {
    return -1;
  }

  } while (1);

  return transferType;
}


DWORD Adm_TransmitICC( DWORD Lun, PUCHAR pucTxBuffer, DWORD ulTxLength, 
		       PUCHAR pucRxBuffer, PDWORD pulRxLength ) {

  DWORD rv;
  UCHAR ucCommand[MAX_BUFFER_SIZE];
  UCHAR ucResponse[MAX_BUFFER_SIZE];
  DWORD ulRecvLength, ulTimeout;
  int i, numberSends;
  int remainingData;
  int transferType;

  ulTimeout = 0;

#define USB_MAXPACKET_SIZE 8

  
  /* Send APDU */

  ucCommand[0] = 0x80; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
  ucCommand[3] = 0x00; ucCommand[4] = 0x00;
  
  
  memcpy(&ucCommand[5], pucTxBuffer, 5);
  
  ControlUSB(Lun, 0, 10, ucCommand, 
	     0, ucResponse);

  transferType = Adm_PollStatus(Lun);

  if ( (transferType == 1) && (ulTxLength > 5) ) {
    printf("Send much data + 2 status bytes\n");
    
    /* Send Data */    
    
    ucCommand[0] = 0x82; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
    ucCommand[3] = 0x00; ucCommand[4] = 0x00;
    
    memcpy(&ucCommand[5], &pucTxBuffer[5], ulTxLength - 5);
    ControlUSB(Lun, 0, ulTxLength, ucCommand, 0, ucResponse);
    
    transferType = Adm_PollStatus(Lun);
    
    if ( transferType == 2 ) {
      ucCommand[0] = 0x81; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
      ucCommand[3] = 0x00; ucCommand[4] = 0x00;
      
      ulRecvLength = 2;
      
      ControlUSB(Lun, 1, 5, ucCommand, 
		 &ulRecvLength, ucResponse);
      
      memcpy( pucRxBuffer, ucResponse, ulRecvLength );
      *pulRxLength = 2;
    }
    
    
  } else if ( transferType == 1 && ulTxLength == 5 ) {
    printf("Receive much data then status bytes\n");

    /* Send Data */    
    numberSends   = (pucTxBuffer[4]) / USB_MAXPACKET_SIZE;
    remainingData = (pucTxBuffer[4]) % USB_MAXPACKET_SIZE;
    
    printf("%d packets %d remaining \n", numberSends, remainingData);

    ucCommand[0] = 0x81; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
    ucCommand[3] = 0x00; ucCommand[4] = 0x00;
    
    ulRecvLength = pucTxBuffer[4];
    
    ControlUSB(Lun, 1, 5, ucCommand, 
	       &ulRecvLength, ucResponse);
    
    transferType = Adm_PollStatus(Lun);

    if ( transferType == 2 ) {
      ucCommand[0] = 0x81; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
      ucCommand[3] = 0x00; ucCommand[4] = 0x00;
      
      ulRecvLength = 2;

      ControlUSB(Lun, 1, 5, ucCommand, 
		 &ulRecvLength, &ucResponse[pucTxBuffer[4]]);
      
      memcpy( pucRxBuffer, ucResponse, pucTxBuffer[4] + 2 );
      *pulRxLength = pucTxBuffer[4] + 2;
    }

    
  } else if ( transferType == 2 ) {
    printf("Get the status bytes only\n");

    ucCommand[0] = 0x81; ucCommand[1] = 0x00; ucCommand[2] = 0x00;
    ucCommand[3] = 0x00; ucCommand[4] = 0x00;
    
    ulRecvLength = 2;
    
    ControlUSB(Lun, 1, 5, ucCommand, 
	       &ulRecvLength, ucResponse);
    
    memcpy( pucRxBuffer, ucResponse, ulRecvLength );
    *pulRxLength = 2;    
    
    return STATUS_SUCCESS;
  }

  return STATUS_SUCCESS;
}

