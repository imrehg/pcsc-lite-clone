#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"

testSIEMENS(int argc,char** argv ) {
unsigned char test2[] = { 0x1,0x2,0x5,0x1E}	;
char dummy[100];
int j=0,i=0;
unsigned char atr[] = { 0x3b ,0xef ,0x0 ,0x0 ,0x81 ,0x31 ,0x20 ,0x49 ,0x0 ,0x5c ,0x50 ,0x43 ,0x54 ,0x10 ,0x27 ,0xf8 ,0xd2 ,0x76 ,0x0 ,0x0 ,0x38 ,0x33 ,0x0 ,0x4d ,0x90,0};
	data = alloca(300);
	buffer = alloca(300);
	//checkATR(atr);
	/* Try to set T=0 protocol */
	printit("Try to set protocol T0");
	rv = SCardReconnect(hCard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0,SCARD_RESET,&protocol);
	fflush(stdout);
	fflush(stderr);
	if( rv!= SCARD_S_SUCCESS ) {
		myprintf("Failed\n");
		return -1;
    }          

	if( protocol == 1) {
		myprintf("Passed \n");
		}
	else {
		myprintf("Failed\n");
	}
	printit("Set Protocol T=1");
	rv = SCardReconnect(hCard,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T1,SCARD_RESET,&protocol);
	if( rv!= SCARD_S_SUCCESS ) { 
		myprintf("Failed\n");
		return -1;
    }          
	if( protocol == 2 ) {
		myprintf("Passed\n");
	}
	else {
		myprintf("Failed\n");
	}
	myprintf("Test No. 1 \n");
	printit("Buffer boundary test");
	dwSendLength= 5;
    GetCommand(0x00,0x84,0x00,0x00,0x08,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T1,0);
//  printf("Passed\n");
	myprintf("Test No. 2 \n");
	printit("SELECT FILE EFwtx");
	fflush(stdout);
	data[0]=0x3E;
	data[1]=0x00;
	data[2]=0x00;
	data[3]=0x01;
	dwSendLength=9;
	GetCommand(0x00,0xA4,0x08,0x04,0x04,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,0);
	dwSendLength=5;
	for(i=0;i<4;i++)
	{
	sprintf(dummy,"READ BINARY %3d Bytes",test2[i]);
	printit(dummy);
	READBIN(0,0,test2[i],data,buffer,dwSendLength);	
	DO_TRANSMIT(SCARD_PCI_T1,0);
	};
	myprintf("Test No. 3\n");
	printit("SELECT FILE EFresync");
	dwSendLength=9;
	data[0]=0x3E;
	data[1]=0x00;
	data[2]=0x00;
	data[3]=0x02;
	GetCommand(0x00,0xA4,0x08,0x04,0x04,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,0);
	printit("WRITE BINARY 255 bytes");
	dwSendLength=255+5;
	for(i=0;i<255;i++)
		data[i]=i;
	UPDATEBIN(0,0,0xFF,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,1);
	printit("READ BINARY 255 Bytes"); 
	dwSendLength=5;
	READBIN(0,0,0xff,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,1);
	myprintf("Test No.  4  \n");
	printit("SELECT FILE EFseqnum");
	data[0]=0x3E;
	data[1]=0x00;
	data[2]=0x00;
	data[3]=0x03;
	dwSendLength=9;
	GetCommand(0x00,0xA4,0x08,0x04,0x04,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,0);
	dwSendLength=5;
	printit("READ BINARY 255 bytes");
	READBIN(0,0,0xff,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,1);
	myprintf("Test No. 5\n");
	printit("SELECT FILE EFifs");
	data[0]=0x3E;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=0x04;
    dwSendLength=9;
    GetCommand(0x00,0xA4,0x08,0x04,0x04,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T1,0);     
	dwSendLength= 0xFF+5;
	for(i=0;i<0xff;i++)
		data[i]=i;
	printit("WRITE BINARY 255 bytes");
	UPDATEBIN(0,0,0xFF,data,buffer,dwSendLength);	
	DO_TRANSMIT(SCARD_PCI_T1,1);
	myprintf("Test No. 6\n");
	printit("SELECT FILE EFtimeout");
	data[0]=0x3E;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=0x05;
    dwSendLength=9;
    GetCommand(0x00,0xA4,0x08,0x04,0x04,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T1,0);   
	dwSendLength=5;
	printit("READ  BINARY 254 bytes");
	READBIN(0,0,0xFE,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,1);
	myprintf("Test No. 7\n");
	printit("SELECT FILE EFresult");
	data[0]=0x3E;
    data[1]=0x00;
    data[2]=0xA0;
    data[3]=0x00;
    dwSendLength=9;
    GetCommand(0x00,0xA4,0x08,0x04,0x04,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T1,0);  	
	printit("READ  BINARY  24 bytes");
	dwSendLength=5;
	READBIN(0,0,24,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,0);
	printit("WTX result");
	if( r[10] == 0) myprintf("Passed \n");
	else myprintf("Failed\n");
	printit("RESYNCH read result");
	if( r[12] == 0) myprintf("Passed \n");
	else myprintf("Failed\n");
	printit("RESYNCH write result");
	if( r[14] == 0) myprintf("Passed \n");
	else myprintf("Failed\n");
	printit("Sequence number result");
	if( r[16] == 0) myprintf("Passed \n");
	else myprintf("Failed\n");
	printit("IFSC request");
	if( r[18] ==0) myprintf("Passed \n"); /* To be checked */
	else myprintf("Failed\n");
	printit("IFSD request");
	if( r[20] == 0) myprintf("Passed \n");
	else myprintf("Failed\n");
	printit("Forced timeout result");
	if( r[22] == 0) myprintf("Passed \n");
		else myprintf("Failed\n");
}
