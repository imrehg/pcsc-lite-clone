#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"
int testAMMI(int argc,char** argv ) {
int j,i=0;
unsigned char test1[] = { 1,25,50,75,100,125,128,150,175,200,225,250,254}	;
unsigned char test2[] = { 0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA}	;
unsigned char test3[] = { 0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF}	;
unsigned char atr[]  = { 0x3b ,0x7e ,0x13 ,0x0 ,0x0 ,0x80 ,0x53 ,0xff ,0xff ,0xff ,0x62 ,0x0 ,0xff ,0x71 ,0xbf ,0x83 ,0x7 ,0x90 ,0x0 ,0x90,0};
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
	data[1]=0x10;
	dwSendLength= 7;
	SELECT(02,data,buffer,dwSendLength);
	printit("SELECT FILE EFptsDataCheck");
	DO_TRANSMIT(SCARD_PCI_T0,0);
	dwSendLength=5;
	printit("READ BINARY 256 Byte(s)");
	READBIN(0,0,0,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
   
	myprintf("Test No. 2\n");
	data[0]=0x00;
    data[1]=0x10;
    dwSendLength= 7;
    SELECT(02,data,buffer,dwSendLength);
    printit("SELECT FILE EFptsDataCheck");
    DO_TRANSMIT(SCARD_PCI_T0,0);
	for(j=0;j<255;j++) 
		data[j]=j;
	printit("WRITE BINARY 255 Byte(s)");
	dwSendLength=255+5;
	UPDATEBIN(0,0,255,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	myprintf("Test No. 3 \n");
	data[0]=0xA0;
	data[1]=0x00;
	dwSendLength= 7;
	SELECT(02,data,buffer,dwSendLength);
	printit("SELECT FILE EFresult");
	DO_TRANSMIT(SCARD_PCI_T0,0);
	printit("READ  BINARY FILE EFResult");
	dwSendLength=5;
	READBIN(0,0,04,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0) ;       
	res=r[1];
	dwSendLength=5;
	printit("READ  BINARY FILE EFresult");
	READBIN(0,0x0A,0x0E,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0) ;       
	printit("\'PTS\'");
	if( r[11] == res ) 
		myprintf("Passed \n");
	else {
		myprintf("Failed \n");
		fails++;
	}
	printit("\'PTS data check\'");
	if( r[13] == res )
		myprintf("Passed \n");
	else {
		myprintf("Failed \n");
		fails++;
	}
	return fails;	
}
