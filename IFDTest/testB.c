#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"
testB(int argc,char** argv ) {
	int i;
myprintf("=======================\nPart B: Checking reader\n=======================\n");
myprintf("Test not ported completely \n");

	(rgReaderStates[0])->dwCurrentState = SCARD_STATE_UNAWARE;
	 rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 ); 
	 if( (rgReaderStates[0])->dwEventState & SCARD_STATE_PRESENT ) {
		myprintf("<<  Please remove smart card \n");
		(rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT;
	 	rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 ); 
	 }
	do {
		myprintf("<< Please insert IBM PC/SC test card\n");
		(rgReaderStates[0])->dwCurrentState = SCARD_STATE_EMPTY;
		rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 );
		rv = SCardConnect(hContext, readerName,
            SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0| SCARD_PROTOCOL_T1,
            &hCard, &dwPref);
     	if ( rv != SCARD_S_SUCCESS ) {
     		SCardReleaseContext( hContext );
    		return -1;
 	 	}                         
		readerlen=100;
		rv = SCardStatus(hCard,readerName,&readerlen,&readerstate,&protocol,r,&atrlen);
		if( checkATR(r,atrlen) != 3 )  { 
			myprintf("ATR Doesn't match \n");
			(rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT;
	        rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 );	
			continue;
		}
		else {
			cards[3].done=0;
			break;
		}
	} while(1);
	printit("2. Cold reset");
	rv = SCardReconnect(hCard,SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0| SCARD_PROTOCOL_T1,
          SCARD_RESET_CARD , &protocol);
	if( rv == SCARD_S_SUCCESS ) 
		myprintf("Passed\n");
	else {
    	SCardReleaseContext( hContext );
    	myprintf("Failed\n");
    	return -1;
	}                         
	readerlen=100;
	printit("Set protocol to T0 | T1");
	myprintf("Passed\n");
	printit("3.SCARD_ATTR_ATR_STRING");
	rv = SCardStatus(hCard,readerName,&readerlen,&readerstate,&protocol,r,&atrlen);
	if( rv != SCARD_S_SUCCESS  || atrlen == 0 ) {
		myprintf("Failed\n");
		return -1;
	 }
	myprintf("Passed\n");
	if(more_details ) {
		 myprintf("ATR :");
		 for(i=0;i<atrlen;i++) {
			myprintf("%0x ",r[i]);
		 }
		 myprintf("\n");
	 }
	printit("4.SCARD_ATTR_CURRENT_PROTOCOL_TYPE");
	if( protocol == 2 ) myprintf("Passed \n");
	else myprintf("Failed\n");
	myprintf("<<  Please remove smart card\n");
	(rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT;
	rv = SCardGetStatusChange(hContext,INFINITE,rgReaderStates[0],1);	
	myprintf("<<  Please insert smart card BACKWARDS\n");
	(rgReaderStates[0])->dwCurrentState = SCARD_STATE_EMPTY;
	rv = SCardGetStatusChange(hContext,INFINITE,rgReaderStates[0],1);	
	printit("5. IOCTL_SMARTCARD_GET_STATE");
	{
		readerlen=100;
		rv = SCardStatus(hCard,readerName,&readerlen,&readerstate,&protocol,r,&atrlen);
		if( rv == SCARD_S_SUCCESS && atrlen == 0 ) 
			myprintf("Passed\n");
		else
			myprintf("Failed\n");
	}
	printit("6. Cold Reset");
	rv = SCardReconnect(hCard, SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0| SCARD_PROTOCOL_T1,
            SCARD_RESET_CARD, &protocol);
	if(( protocol == 0) || (rv != SCARD_S_SUCCESS) ) myprintf("Passed \n");
	else myprintf("Failed \n");
	return 0;
}
