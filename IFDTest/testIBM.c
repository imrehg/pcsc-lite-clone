#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"
int testIBM(int argc,char** argv ) {
unsigned char test1[] = { 1,25,50,75,100,125,128,150,175,200,225,250,254}	;
unsigned char test2[] = { 0x55,0x55,0xAA,0xAA,0x55,0x55,0xAA,0xAA}	;
unsigned char test3[] = { 0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF}	;
unsigned char atr[] = { 0x3b ,0xef, 0x0, 0xff ,0x81 ,0x31 ,0x86 ,0x45 ,0x49 ,0x42 ,0x4d ,0x20 ,0x4d ,0x46 ,0x43 ,0x34 ,0x30 ,0x30 ,0x30 ,0x30 ,0x38 ,0x33 ,0x31 ,0x43 ,0x90,0};
int j,i=0;
char dummy[100];
	data = alloca(300);
	buffer = alloca(300);
	//checkATR(atr);
	/* Try to set T=0 protocol */
	printit("Try to set protocol T0");
	data[0]=0x00;
	data[1]=0x07;
	dwSendLength= 7;
	SELECT(02,data,buffer,dwSendLength);
	DO_TRANSMIT(SCARD_PCI_T0,1);
	myprintf("Set protocol T1 \n");
	myprintf("Test No. 1 \n");
/* Test1 */	
	data[0]=0x00;
	data[1]=0x07;
	dwSendLength= 7;
	SELECT(02,data,buffer,dwSendLength);
	printit("SELECT FILE 007");
	DO_TRANSMIT(SCARD_PCI_T1,0);
	for(i=0;i<13;i++) {
//		myprintf("i = %0x\n",i);
		dwSendLength=test1[i]+5;
//		myprintf("length = %0x\n",dwSendLength);
		for(j=0;j<test1[i];j++)
			data[j]=j;
 	 	UPDATEBIN(0,0,test1[i],data,buffer,dwSendLength);
		sprintf(dummy,"WRITE BINARY %3d byte(s)",test1[i]);
		printit(dummy);
		if( test1[i] >= 150) 
		{
			DO_TRANSMIT(SCARD_PCI_T1,1);
		}
		else
		{
			DO_TRANSMIT(SCARD_PCI_T1,0);
		}
		dwSendLength= 5;
  		READBIN(0,0,test1[i],data,buffer,dwSendLength);
		sprintf(dummy,"READ  BINARY %3d byte(s)",test1[i]);
		printit(dummy);
		DO_TRANSMIT(SCARD_PCI_T1,0) ;
	}
/* Test 2 */
myprintf("Test No. 2 \n");
		data[0]=0x00;
		data[1]=0x07;
		dwSendLength= 7;
		SELECT(02,data,buffer,dwSendLength);
		printit("SELECT FILE 007");
		DO_TRANSMIT(SCARD_PCI_T1,0);
		for(i=0;i<8;i++) {
			for(j=0;j<128;j++) 
				data[j]=test2[i];
			sprintf(dummy,"WRITE BINARY %3d Byte(s) Pattern %02xh",128,test2[i]);
			printit(dummy);
			dwSendLength=128+5;
			UPDATEBIN(0,0,128,data,buffer,dwSendLength);
			DO_TRANSMIT(SCARD_PCI_T1,0);
			sprintf(dummy,"READ  BINARY %3d Byte(s) Pattern %02xh",128,test2[i]);
			printit(dummy);
			dwSendLength= 5;
			READBIN(0,0,128,data,buffer,dwSendLength);
			DO_TRANSMIT(SCARD_PCI_T1,0) ;       
		}
myprintf("Test No. 3 \n");
		data[0]=0x00;
		data[1]=0x07;
		dwSendLength= 7;
		SELECT(02,data,buffer,dwSendLength);
		printit("SELECT FILE 007");
		DO_TRANSMIT(SCARD_PCI_T1,0);
		for(i=0;i<8;i++) {
			for(j=0;j<128;j++) 
				data[j]=test3[i];
			sprintf(dummy,"WRITE BINARY %3d Byte(s) Pattern %02xh",128,test3[i]);
			printit(dummy);
			dwSendLength=128+5;
			UPDATEBIN(0,0,128,data,buffer,dwSendLength);
			DO_TRANSMIT(SCARD_PCI_T1,0);
			dwSendLength= 5;
			sprintf(dummy,"READ  BINARY %3d Byte(s) Pattern %02xh",128,test3[i]);
			printit(dummy);
			READBIN(0,0,128,data,buffer,dwSendLength);
			DO_TRANSMIT(SCARD_PCI_T1,0) ;       
		}
	myprintf("Test No. 4 \n");
		/* Select non existing file */
	 	data[0]=0x77;
        data[1]=0x77;
        dwSendLength= 7;
        SELECT(02,data,buffer,dwSendLength);
        printit("SELECT NONEXISTING FILE");
        DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No. 5 \n");
		dwSendLength=4;
		SELECT(00,data,buffer,dwSendLength);
		printit("SELECT COMMAND WITHOUT FILEID");
		DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No. 6 \n");
		printit("SELECT COMMAND PATH WITH PATH TOO SHORT");
		data[0]=77;
		dwSendLength=6;
		SELECT(01,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No. 7 \n");
		printit("SELECT COMMAND PATH WITH WRONG LC");
		for(i=0;i<8;i++) 
			data[i]=0;
		dwSendLength=06;
		SELECT(8,data,buffer,dwSendLength);		
		DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No.  8\n");
		printit("SELECT COMMAND TOO SHORT");
		dwSendLength=3;
		SELECT(0,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No.  9\n");
		printit("SELECT COMMAND WITH INVALID P2");
		dwSendLength=7;
		data[0]=00;
		data[1]=07;
		/* a modification in select - use getcommand directly */
		GetCommand(00,0xA4,00,02,02,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No. 10 \n");
		printit("SELECT COMMAND WITHOUT FILEID BUT WITH Le");
		dwSendLength=5;
		SELECT(0,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T1,1);
		myprintf("Test No. 11 \n");
		printit("SELECT FILE 0007");
		data[0]=0x00;
        data[1]=0x07;
        dwSendLength= 7;
        SELECT(02,data,buffer,dwSendLength);
        DO_TRANSMIT(SCARD_PCI_T1,0);
		printit(" CHANGE SPEED");
		/* don't know what to do. Blindly copied from NT output */
		dwSendLength=4;
		GetCommand(0xB6,0x42,0x00,0x40,00,data,buffer,dwSendLength);
		DO_TRANSMIT(SCARD_PCI_T1,0);
		printit(" SELECT FILE 0007 WILL GET NO VALID RESPONSE");
		data[0]=0x00;
		data[1]=0x07;
   		dwSendLength= 7;
       	SELECT(02,data,buffer,dwSendLength);
       	DO_TRANSMIT(SCARD_PCI_T1,1);
		return fails;
}
