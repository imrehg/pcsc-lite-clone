
/******************************************************************

	MUSCLE SmartCard Development ( http://www.linuxnet.com )
	    Title  : test.c
	    Package: pcsc lite
            Author : David Corcoran
            Date   : 7/27/99
	    License: Copyright (C) 1999 David Corcoran
	             <corcoran@linuxnet.com>
            Purpose: This is a test program for pcsc-lite.
	            
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pcscdefines.h>
#include <ifdhandler.h>   

void printit(int len , unsigned char dummy[300]) {
	int i;
	for(i=0;i<len;i++) {
		myprintf("%0x ",dummy[i]);
	}
	myprintf("\n");
}
int main( int argc, char **argv ) {
unsigned char dummy[300];
unsigned long len;
	long dummyLong;
  int t = 0;
  int rv;
  myprintf("\nWinscard PC/SC Lite Test Program\n\n");

  myprintf("Testing SCardEstablishContext \n");
  rv = IFDHCreateChannel(0,0);
	if( rv != IFD_SUCCESS ) 
		{
		myprintf("unable to open channel \n");
		return -1;
	}
	rv=0;
	myprintf("waiting for card to be inserted \n");
	while( rv != IFD_ICC_PRESENT ) {
  		rv = IFDHICCPresence(0); 
	}
	myprintf("Card in slot \n");
	rv = IFDHPowerICC(0,IFD_POWER_UP,dummy,&len);
	if( rv != IFD_SUCCESS ) 
		myprintf("error in power up \n");
	printit(len,dummy);
	rv = IFDHGetCapabilities(0,TAG_IFD_ATR,&len,dummy);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("ATR :");
	printit(len,dummy);
	rv = IFDHGetCapabilities(0,0x0100,&len,dummy);
	myprintf("rv = %d %d Vendor :%s \n",rv,len,dummy);
	rv = IFDHGetCapabilities(0,0x0101,&len,dummy);
	myprintf("rv = %d %d IFD_Type %s\n",rv,len,dummy);
	rv = IFDHGetCapabilities(0,0x0103,&len,dummy);
	myprintf("rv = %d %d IFD_Serial %s \n",rv,len,dummy);
	rv = IFDHGetCapabilities(0,0x0300,&len,dummy);
	myprintf("rv = %d %d iccpresence %0x \n",rv,len,dummy[0]);
	rv = IFDHGetCapabilities(0,0x0301,&len,dummy);
	myprintf("rv = %d %d interface status %0x \n",rv,len,dummy[0]);
	rv = IFDHGetCapabilities(0,0x0304,&len,dummy);
	myprintf("rv = %d %d ICC_Type %0x \n",rv,len,dummy[0]);
	rv = IFDHGetCapabilities(0,0x0102,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("IFD_VERSION %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0120,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("ASYNC_SUPPORTED %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0121,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("DEFAULT CLK %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0122,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("MAX_CLK %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0123,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("DEFAULT_DATA_RATE %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0124,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("MAX_DATA_RATE %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0125,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("MAX_IFSD %ld \n",dummyLong);
	rv = IFDHGetCapabilities(0,0x0201,&len,&dummyLong);
	myprintf("rv = %d %d \n",rv,len);
	myprintf("PROTOCOL_TYPE %ld \n",dummyLong);
}	

