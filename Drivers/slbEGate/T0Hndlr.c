/*****************************************************************
/
/ File   :   T0Hndlr.c
/ Author :   David Corcoran
/ Date   :   October 15, 1999
/ Purpose:   This provides a T=0 handler.
/            See http://www.linuxnet.com for more information.
/ License:   See file LICENSE
/
******************************************************************/

#include "pcscdefines.h"
#include "T0Hndlr.h"
#include "AdmHndlr.h"
#include <unistd.h>
#include <stdio.h>

DWORD T0_ExchangeData( DWORD ulSlot, PUCHAR pRequest, DWORD RequestLen, 
                       PUCHAR pReply, PDWORD pReplyLen ) {

//  int i;
//  int len;
  int rv;
  int firstTry = 0;
  int bytesWritten, bytesToWrite;
  int totalBytesToWrite;
  int isoApduCase; 
  unsigned long rspSize;
//  unsigned char rxBuffer[MAX_BUFFER_SIZE];
  unsigned char rsp[MAX_BUFFER_SIZE];
  unsigned char cmd[MAX_BUFFER_SIZE];
  unsigned char *rspSpot;



  rv = Adm_TransmitICC( ulSlot, pRequest, RequestLen, 
			pReply, pReplyLen );

     return rv;

  /* Adm_SetWWT(ADM_DEFAULT_WWT); */

  /* Determine the type of APDU to be sent */

  if ( (RequestLen == 5) && (pRequest[4] == 0x00) ) {
    isoApduCase = 1;  /* ISO 7816 Case 1 Transaction */
  } else if ( (RequestLen == 5) && (pRequest[4] > 0x00) ) {
    isoApduCase = 2;  /* ISO 7816 Case 2 Transaction */
  } else if ( (RequestLen > 5) && (pRequest[4] > 0x00) ) {
    isoApduCase = 3;  /* ISO 7816 Case 3 Transaction */
  } else if ( (RequestLen > 5) && ((RequestLen) == 
              (pRequest[4] + 5 + 1)) ) {
    isoApduCase = 4;  /* ISO 7816 Case 3 Transaction */
  }

  bytesWritten      = 0;
  totalBytesToWrite = RequestLen;

  bytesToWrite = 5;

    do {

    rspSize = 1;  /* Only get 1 byte from the card */

    memcpy( cmd, &pRequest[bytesWritten], bytesToWrite );

    if ( firstTry == 0 ) {
    rv = Adm_TransmitICC( ulSlot, cmd, 50000, 
			  rsp, &rspSize );
    
    firstTry = 1;
    } else {
      rv = Adm_TransmitICC( ulSlot, cmd, bytesToWrite, 
			    rsp, &rspSize );
    }

    if ( rv != STATUS_SUCCESS ) {
      *pReplyLen = 0;
      return rv;
    }
    
    bytesWritten      += bytesToWrite;
    totalBytesToWrite -= bytesToWrite;

    if (rsp[0] == 0x60) {
      usleep(100000);    /* Sleep for a bit     */
      bytesToWrite = 0; /* Just read this time */
      continue;
      
    } else if ( rsp[0] == cmd[1] ) {
      if ( isoApduCase == 1 ) {
        rspSize = 2;
        rv = Adm_TransmitICC( ulSlot, cmd, 0, 
			      rsp, &rspSize );
        if (rv != STATUS_SUCCESS) {
          *pReplyLen = 0;
          return rv;
        }

        *pReplyLen = 2;
        memcpy( pReply, rsp, rspSize );
        return rv;

      } else if ( isoApduCase == 2 ) {
        rspSize = pRequest[4] + 2;
        rv = Adm_TransmitICC( ulSlot, cmd, 0, 
			      rsp, &rspSize );
                                                            
        if (rv != STATUS_SUCCESS) {
          *pReplyLen = 0;
          return rv;
        }

        *pReplyLen = rspSize;
        memcpy( pReply, rsp, rspSize );
        return rv;

      } else if ( isoApduCase == 3 ) {

        rspSize = 1;
        memcpy( cmd, &pRequest[bytesWritten], totalBytesToWrite );
        rv = Adm_TransmitICC( ulSlot, cmd, totalBytesToWrite, 
                              rsp, &rspSize );
                
       if (rv != STATUS_SUCCESS) {
            *pReplyLen = 0;
            return rv;
        }
                                                
        if (rsp[0] == 0x60) {
            do {
                usleep(100000);
            
                rspSize = 1;
                rv = Adm_TransmitICC( ulSlot, cmd, 0, 
                                      rsp, &rspSize );       
                if (rv != STATUS_SUCCESS) {
                    *pReplyLen = 0;
                    return rv;
                }

            } while (rsp[0] == 0x60);
        }
        
        rspSize = 1;
        rv = Adm_TransmitICC( ulSlot, cmd, 0, 
                              &rsp[1], &rspSize );         
               
        if (rv != STATUS_SUCCESS) {
            *pReplyLen = 0;
            return rv;
        }
        
        rspSize = 2;
        
        *pReplyLen = 2;
        memcpy( pReply, rsp, rspSize );
        return rv;
        
      } else if ( isoApduCase == 4 ) {
        /* Stupid ass ISO 7816 brain dead transaction */
      } else {
        /* We have a problem here.... */
        *pReplyLen = 0;
        return STATUS_UNSUCCESSFUL;
      }

   } else if ( (rsp[0] == ~cmd[1]) || 
               (rsp[0] == ~(cmd[1] + 1)) ) {
      
      if ( isoApduCase == 1 ) {
         /* This will never happen */
         *pReplyLen = 0;
         return STATUS_UNSUCCESSFUL;
       } else if ( isoApduCase == 2 ) {
         /* we can only receive the next byte */
         bytesToWrite = 0;
         continue;
       } else if ( isoApduCase == 3 ) {
         /* we can only send the next byte */
         bytesToWrite = 1;
         continue;
       } else if ( isoApduCase == 4 ) {
        /* ISO 7816 case 4 transaction */
       }

   } else if (( rsp[0] >= 0x61 && rsp[0] <= 0x6F ) ||
             ( rsp[0] >= 0x90 && rsp[0] <= 0x9F )) {
     
     pReply[0] = rsp[0];

     rspSize = 1;
     rv = Adm_TransmitICC( ulSlot, cmd, 0, 
			   rsp, &rspSize );
                           
     if (rv != STATUS_SUCCESS) {
        *pReplyLen = 0;
        return rv;
     }

     *pReplyLen = 2;
     pReply[1] = rsp[0];

     return rv;
   } else {
     /* Serious problem here */
   }

  } while (1);


  return STATUS_SUCCESS;
}


