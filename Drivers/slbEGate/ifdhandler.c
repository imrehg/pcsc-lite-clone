/*****************************************************************
/
/ File   :   ifdhandler.c
/ Author :   David Corcoran <corcoran@linuxnet.com>
/ Date   :   June 15, 2000
/ Purpose:   This provides reader specific low-level calls.
/            Alot of the functionality listed in the specs is
/            not done.  I've done a minimum to get started.
/            See http://www.linuxnet.com for more information.
/ License:   See file LICENSE
/
******************************************************************/

#include "pcscdefines.h"
#include "ifdhandler.h"
#include "AdmHndlr.h"
#include "T0Hndlr.h"
#include <stdio.h>

/* #define PCSC_DEBUG 1 */

static struct _IFDCard {
  PUCHAR Atr[MAX_ATR_SIZE];
  DWORD  AtrLength;
} IFDCard;

RESPONSECODE IFDHCreateChannel ( DWORD Lun, DWORD Channel ) {

  ULONG rv, rvb;

  rv = OpenUSB(Lun);

  if ( rv != STATUS_SUCCESS ) {
    return IFD_COMMUNICATION_ERROR;
  }

  rv  = Adm_ResetICC( Lun, IFDCard.Atr, &IFDCard.AtrLength ); 

  if ( rv == STATUS_SUCCESS ) {
    return IFD_SUCCESS;
  } else {
    return IFD_COMMUNICATION_ERROR;
  }
  
}


RESPONSECODE IFDHCloseChannel ( DWORD Lun ) {
  
  return IFD_SUCCESS;   
  
}

RESPONSECODE IFDHGetCapabilities ( DWORD Lun, DWORD Tag, 
				   PDWORD Length, PUCHAR Value ) {
  
  switch ( Tag ) {

  case TAG_IFD_ATR:
    memcpy( Value, IFDCard.Atr, IFDCard.AtrLength );
    *Length = IFDCard.AtrLength;
    break;

  default:
    return IFD_ERROR_TAG;

  }

  return IFD_SUCCESS;
}

RESPONSECODE IFDHSetCapabilities ( DWORD Lun, DWORD Tag, 
				   DWORD Length, PUCHAR Value ) {

  return IFD_SUCCESS;
}

RESPONSECODE IFDHSetProtocolParameters ( DWORD Lun, DWORD Protocol, 
				   UCHAR Flags, UCHAR PTS1,
				   UCHAR PTS2, UCHAR PTS3) {

  return IFD_SUCCESS;
}


RESPONSECODE IFDHPowerICC ( DWORD Lun, DWORD Action, 
			    PUCHAR Atr, PDWORD AtrLength ) {
ULONG rv;

  if ( Action == IFD_POWER_UP ) {
        rv = Adm_ResetICC( Lun, Atr, AtrLength ); 
       if ( rv == STATUS_SUCCESS ) {
         memcpy( IFDCard.Atr, Atr, *AtrLength );
	 IFDCard.AtrLength = *AtrLength;
         return IFD_SUCCESS;
      } else {
         return IFD_COMMUNICATION_ERROR;
      }

  } else if ( Action == IFD_POWER_DOWN ) {
    /* Can't do much here */
       if ( rv == STATUS_SUCCESS ) {
         return IFD_SUCCESS;
      } else {
         return IFD_COMMUNICATION_ERROR;
      }
    
  } else if ( Action == IFD_RESET ) {
    rv = Adm_ResetICC( Lun, Atr, AtrLength ); 
    if ( rv == STATUS_SUCCESS ) {
      memcpy( IFDCard.Atr, Atr, *AtrLength );
      IFDCard.AtrLength = *AtrLength;
      return IFD_SUCCESS;
    } else {
      return IFD_COMMUNICATION_ERROR;
    }
    
  } else {
    return IFD_NOT_SUPPORTED;
  }

  return IFD_SUCCESS;
}

RESPONSECODE IFDHTransmitToICC ( DWORD Lun, SCARD_IO_HEADER SendPci, 
				 PUCHAR TxBuffer, DWORD TxLength, 
				 PUCHAR RxBuffer, PDWORD RxLength, 
				 PSCARD_IO_HEADER RecvPci ) {
  
  ULONG rv;
  int i;

#ifdef PCSC_DEBUG
   printf("T=%d -> ", SendPci.Protocol);
   for (i=0; i < TxLength; i++) {
     printf("%x ", TxBuffer[i]);
   } printf("\n");
#endif

  if ( SendPci.Protocol == 0 ) {
    rv = T0_ExchangeData( Lun, TxBuffer, TxLength, RxBuffer, RxLength );
  } else {
    return IFD_PROTOCOL_NOT_SUPPORTED;
  }

#ifdef PCSC_DEBUG
   printf("T=%d <- ", SendPci.Protocol);
   for (i=0; i < *RxLength; i++) {
     printf("%x ", RxBuffer[i]);
   } printf("\n");
#endif

  if ( rv == STATUS_SUCCESS ) {
    return IFD_SUCCESS;
  } else {
    return IFD_COMMUNICATION_ERROR;
  }
}

RESPONSECODE IFDHControl ( DWORD Lun, PUCHAR TxBuffer, 
			 DWORD TxLength, PUCHAR RxBuffer, 
			 PDWORD RxLength ) {

  /* This function performs a data exchange with the reader (not the card)
     specified by Lun.  Here XXXX will only be used.
     It is responsible for abstracting functionality such as PIN pads,
     biometrics, LCD panels, etc.  You should follow the MCT, CTBCS 
     specifications for a list of accepted commands to implement.

     TxBuffer - Transmit data
     TxLength - Length of this buffer.
     RxBuffer - Receive data
     RxLength - Length of the received data.  This function will be passed
     the length of the buffer RxBuffer and it must set this to the length
     of the received data.

     Notes:
     RxLength should be zero on error.
  */

  return IFD_SUCCESS;
}

RESPONSECODE IFDHICCPresence( DWORD Lun ) {

 return IFD_ICC_PRESENT;      

}
