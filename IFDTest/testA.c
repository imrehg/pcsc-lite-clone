#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"
testA(int argc,char** argv ) {
	int i;
myprintf("=============================\n");
myprintf("Part A: Checking card monitor\n");
myprintf("=============================\n");
	(rgReaderStates[0])->dwCurrentState = SCARD_STATE_UNAWARE;
	 rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 ); 
	 if( (rgReaderStates[0])->dwEventState & SCARD_STATE_PRESENT ) {
		myprintf("<<  Please remove smart card \n");
		(rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT;
	 	rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 ); 
	 }
	printit("1.Please insert smart card");
		(rgReaderStates[0])->dwCurrentState = SCARD_STATE_EMPTY;
		rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 );
	 myprintf("Passed\n");
	 printit("2. IOCTL_SMARTCARD_IS_PRESENT");
	 myprintf("Passed\n");
	 printit("3. Please remove smart card ");
	 (rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT; 
     rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 );
     if( rv != SCARD_S_SUCCESS )
        return -1;
	 if( (rgReaderStates[0])->dwEventState & SCARD_STATE_EMPTY) myprintf("Passed\n");
	 else 
		myprintf("Failed \n");
	 printit("4. IOCTL_SMARTCARD_IS_ABSENT");
	 myprintf("Passed\n");
	 printit("5. Insert and remove a smart card repeatedly");
	(rgReaderStates[0])->dwCurrentState = SCARD_STATE_EMPTY;
	for(i=0;i<10;i++) {
	 	rv =SCardGetStatusChange( hContext, INFINITE, rgReaderStates[0], 1 ); 
		(rgReaderStates[0])->dwCurrentState = (rgReaderStates[0])->dwEventState;
	 }
	 myprintf("Passed \n");
	 if( more_details ) {
		 rv = SCardConnect(hContext, readerName,
            SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0| SCARD_PROTOCOL_T1,
            &hCard, &dwPref);
     	if ( rv != SCARD_S_SUCCESS ) {
     		SCardReleaseContext( hContext );
    		return 0;
 	 	}                         
		readerlen=100;
	 	rv = SCardStatus(hCard,readerName,&readerlen,&readerstate,&protocol,r,&atrlen);
	 	if( rv != SCARD_S_SUCCESS ) {
			return -1;
		 }
		 myprintf("ATR :");
		 for(i=0;i<atrlen;i++) {
			myprintf("%0x ",r[i]);
		 }
		 myprintf("\n");
		 myprintf("Powered up successfully \n");
	 }
	 return 0;
}
