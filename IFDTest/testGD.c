#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"

testGD(int argc,char** argv ) {
unsigned char atr[] = { 0x3b ,0xbf ,0x18 ,0x0 ,0x80 ,0x31 ,0x70 ,0x35 ,0x53 ,0x54 ,0x41 ,0x52 ,0x43 ,0x4f ,0x53 ,0x20 ,0x53 ,0x32 ,0x31 ,0x20 ,0x43 ,0x90 ,0x0 ,0x9b ,0x90,0 };
int k,l,j,i=0;
	data = alloca(300);
	buffer = alloca(300);
	//checkATR(atr);
	/* Try to set T=0 protocol */
	printit("Try to set protocol T0 | T1");
	rv = SCardReconnect(hCard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0,SCARD_RESET,&protocol);
	if( rv != SCARD_S_SUCCESS ) { 
		myprintf("Failed\n");
		return -1;
    }          
	rv = SCardReconnect(hCard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T1,SCARD_RESET,&protocol);
	if( rv != SCARD_S_SUCCESS ) { 
		myprintf("Failed\n");
		return -1;
    }          
	myprintf("Passed\n");
	myprintf("Test No. 1 \n");
	printit("Cold reset");
	rv = SCardReconnect(hCard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0,SCARD_RESET,&protocol);
    if( rv!= SCARD_S_SUCCESS ) {
		myprintf("Failed\n");
		return -1;
    	}       
	myprintf("Passed\n");
	printit("Set protocol T=0");
	myprintf("Passed \n");
	printit("SELECT FILE EFptsDataCheck");
	data[0]=00;
	data[1]=01;
	dwSendLength=07;
	SELECT(02,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	printit("WRITE BINARY 256 bytes");
	l=0;
	k=0;
	for(i=0;i<4;i++) {
		for(l=0x40*i;l<0x40*(i+1);l++)
			data[k++]=l;
		k=0;
		dwSendLength=0x45;
		UPDATEBIN(0,i*0x40,0x40,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T0,0);
	}
	printit("Cold reset");
    rv = SCardReconnect(hCard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T1,SCARD_RESET,&protocol);
    if( rv != SCARD_S_SUCCESS ) {
		myprintf("Failed\n");
		return -1;
    }
    myprintf("Passed\n");
    printit("Set protocol T=1");
	myprintf("Passed\n");
    printit("SELECT FILE EFptsDataCheck");
    data[0]=00;
    data[1]=01;
    dwSendLength=07;
    SELECT(02,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T1,0);
	dwSendLength=5;
	printit("READ BINARY 256 Bytes");
	for(i=0;i<4;i++)
	{
		READBIN(0,i*0x40,0x40,data,buffer,dwSendLength);	
		DO_TRANSMIT(SCARD_PCI_T1,0);
	};
}
