#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"
int temp1;

testBULL(int argc,char** argv ) {
unsigned char test1[] = { 1,25,50,75,100,125,128,150,175,200,225,250,254}	;
unsigned char test2[] = { 0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA}	;
unsigned char test3[] = { 0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF}	;
unsigned char atr[] = { 0x3f ,0x67 ,0x25 ,0x0 ,0x21 ,0x20 ,0x0 ,0xf ,0x68 ,0x90 ,0x0,0x90,0}	;
int l,j,i=0;
char dummy[100];
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
	myprintf("Test No. 1 \n\n");
/* Test1 */	
	printit("Buffer boundary test"); 
	dwSendLength= 5;
	GetCommand(0x00,0x84,0x00,0x00,0x08,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,1); /* Changed to 1 - getting 6d,0 */
	myprintf("Test No. 2\n");
	printit("3 byte APDU");
	dwSendLength= 3;
	GetCommand(0x00,0x84,0x00,0x00,0x08,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,1);
	myprintf("Test No. 3\n");
	printit("GET CHALLENGE");
	dwSendLength=5;	
	GetCommand(0,0xC4,0,0,8,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	printit("VERIFY PIN");
	dwSendLength=0x0f;
	for(i=0;i<0x0a;i++) 
		data[i]=i+1;
	GetCommand(0x00,0x38,0,0,0xA,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	myprintf("Test No. 4\n");
	printit("SEARCH BLANK WORD");
	dwSendLength=5;
	GetCommand(0x00,0xA0,0,0,0,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	printit("GET RESPONSE");
	dwSendLength=5;
	GetCommand(0,0xC0,0,0,8,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	myprintf("Test No.  5\n");
	printit("Lc byte incorrect");
	dwSendLength=6;
	data[0]=0x2f;
	data[1]=0x01;
	GetCommand(0,0xA4,0,0,3,data,buffer,dwSendLength); 	
	DO_TRANSMIT(SCARD_PCI_T0,1);
	myprintf("Test No. 6\n");
	printit("SELECT FILE");
	data[0]=0x2f;
	data[1]=0x01;
	dwSendLength=7;
	SELECT(2,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	printit("ERASE BINARY");
	dwSendLength=7;
	data[0]=00;
	data[1]=0x78;
	GetCommand(0x00,0x0E,0,0,2,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	l=0;
	for(i=0x00;i<0x41;i++) {
		sprintf(dummy,"WRITE BINARY - 4 bytes (%03d)",i);
		printit(dummy);
		dwSendLength=9;
			data[0]=l;
			data[1]=l+1;
			data[2]=l+2;
			data[3]=l+3;
			l+=4;	
		GetCommand(0x00,0xD0,0,i,4,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T0,0);
	}
	printit("READ BINARY - 256 bytes");
	dwSendLength=5;
	READBIN(0,0,0,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	myprintf("Test No. 7\n");
	printit("GENERATE TEMP KEY");
	dwSendLength=7;
	data[0]=0x12;
	data[1]=0x00;
	GetCommand(0x00,0x80,0,0,2,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	myprintf("Test No. 8\n");
	printit("SELECT FILE");
	dwSendLength=7;
	data[0]=0x3F;
	data[1]=0x00;
	SELECT(2,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,0);
	printit("ERASE BINARY");
	dwSendLength=7;
	data[0]=0;
	data[1]=0x78;
	GetCommand(0x00,0x0E,0,0,2,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,1);  /* changed to 1 - getting 67,9 */
}
