#include <stdio.h> 
#include <winscard.h> 
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"

testSCHL(int argc,char** argv ) {
unsigned char test1[] = { 1,25,50,75,100,125,128,150,175,200,225,250,254}	;
unsigned char test2[] = { 0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA}	;
unsigned char test3[] = { 0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF}	;
unsigned char atr[] = { 0x3b ,0xe2 ,0x0 ,0x0 ,0x40 ,0x20 ,0x99 ,0x1 ,0x90,0 };
int j,i=0;
int res;
	data = alloca(300);
	buffer = alloca(300);
	//checkATR(atr);
	/* Try to set T=0 protocol */
	printit("Try to set protocol T1");
	data[0]=0x00;
	data[1]=0x07;
	dwSendLength= 7;
	SELECT(02,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T1,1);
	myprintf("Set protocol T0 \n");
	myprintf("Test No. 1 \n");
/* Test1 */	
	data[0]=0x00;
	data[1]=0x01;
	dwSendLength= 7;
	SELECT(02,data,buffer,dwSendLength);
	printit("SELECT FILE EFtransferAllBytes");
	DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	printit("WRITE BINARY   1 Byte(s)");
	data[0]=0;
	dwSendLength=06;
	UPDATEBIN(0,0,1,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
  	printit("WRITE BINARY  25 Byte(s)");
    dwSendLength=0x1E;
	for(i=0;i<0x19;i++)
		data[i]=i;
    UPDATEBIN(0,0,0x19,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	myprintf("Test No. 2\n");
    data[0]=0x00;
    data[1]=0x02;
    dwSendLength= 7;
    SELECT(02,data,buffer,dwSendLength);
    printit("SELECT FILE EFtransferNextByte");
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
    printit("WRITE BINARY   1 Byte(s)");
    data[0]=0;
    dwSendLength=06;
    UPDATEBIN(0,0,1,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
    printit("WRITE BINARY  25 Byte(s)");
    dwSendLength=0x1E;
    for(i=0;i<0x19;i++)
        data[i]=i;
    UPDATEBIN(0,0,0x19,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	myprintf("Test No.  3\n");
	printit("SELECT FILE EFread256Bytes");
	data[0]=0x00;
    data[1]=0x03;
    dwSendLength= 7;
    SELECT(02,data,buffer,dwSendLength);     	
	 DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	dwSendLength=5;
	printit(" READ BINARY 256 Byte(s)");
	READBIN(0,0,0,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	myprintf("Test No.  4\n");
	printit("SELECT FILE EFcase1Apdu");
	data[0]=0x00;
    data[1]=0x04;
    dwSendLength= 7;
    SELECT(02,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	printit("WRITE BINARY   0 Byte");
	dwSendLength=5;
	UPDATEBIN(0,0,0,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	myprintf("Test No.  5 \n");
	printit("SELECT FILE EFrestartWorkWaitingTime");
	data[0]=0x00;
    data[1]=0x05;
    dwSendLength= 7;
    SELECT(02,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	printit("READ BINARY   1 Byte(s)");
	dwSendLength=5;
	READBIN(0,0,1,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	 printit("READ BINARY   2 Byte(s)");
    dwSendLength=5;
    READBIN(0,0,2,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
    printit("READ BINARY   5 Byte(s)");
    dwSendLength=5;
    READBIN(0,0,5,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
    printit("READ BINARY  30 Byte(s)");
    dwSendLength=5;
    READBIN(0,0,30,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	myprintf("Test No.  6\n");
	printit("SELECT FILE EFresult");
	data[0]=0xa0;
    data[1]=0x00;
    dwSendLength= 7;
    SELECT(02,data,buffer,dwSendLength);
    DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	printit("READ  BINARY FILE EFresult");
	dwSendLength=5;
	READBIN(0,0,4,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	sleep(1);
	res = r[1];
	printit("Transfer all remaining bytes result");
	dwSendLength=5;
	READBIN(0,10,0x0E,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
		myprintf("\'Transfer next byte result\'");
	if( r[7] == res )  
		myprintf("Passed\n");
	else 
		myprintf("Failed\n");
		printit("\'Read 256 bytes bytes\' result");
	if( r[9] == res ) 
		myprintf("Passed\n");
	else	
		myprintf("Failed\n");
	printit("\'Case 1 APDU\' result");
	if( r[11] == res ) 
		myprintf("Passed\n");
	else
		myprintf("Failed\n");
	printit("\'Restart of work waiting time\' result");
	if( r[13] == res )
		myprintf("Passed \n");
	else
		myprintf("Failed\n");
	sleep(1);
}
