
#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include <stdarg.h>  
#include "commands.h"

int myprintf(const char* fmt , ...) {
		va_list va;
		va_start(va,fmt);
		vprintf(fmt,va);
		if( fp != NULL) 
			vfprintf(fp,fmt,va);
		va_end(va);
	}	
void printit(char cmd[] ) {
int i;
int l;
myprintf("%s",cmd);
l=strlen(cmd);
for(i=0;i<60-l;i++)
	myprintf(" ");
fflush(stdout);
}	
void GetCommand(unsigned char class , unsigned char ins , unsigned char p1,unsigned char p2 ,unsigned char size , unsigned char * data , unsigned char *buffer,int length)  {
	buffer[0]=class;
	buffer[1]=ins ;
	buffer[2]=p1;
	buffer[3]=p2;
	buffer[4]=size;
	/*myprintf("len = %0x \n",length); */
	if( length > 5 ) 
	memcpy(buffer+5,data,size);
	
}
int checkResult(int rv , unsigned char r[300],int len,int neg) {
	int result,i;	
  if( rv == SCARD_S_SUCCESS ) {
//        myprintf("SCARD_S_SUCCESS\n");
			result = (r[len-2] == 0x90 ||  r[len-2] == 0x61 || r[len-2] == 0x62 || r[len-2] == 0x63 );
	//		myprintf("r[len-2] = %0x result = %d neg = %d ",r[len-2] , result,neg);
			return (neg == result);
    }
    else if ( rv == SCARD_E_INVALID_HANDLE ) {
 //       myprintf("SCARD_E_INVALID_HANDLE \n");
		return !neg & 1;
    }
    else if ( rv == SCARD_E_NOT_TRANSACTED ) {
  //      myprintf ( " SCARD_E_NOT_TRANSACTED \n");
		return !neg & 1;
    }
    else if ( rv == SCARD_E_READER_UNAVAILABLE ) {
   //     myprintf("SCARD_E_READER_UNAVAILABLE \n");
		return !neg & 1;
    }
    else if ( rv == SCARD_E_PROTO_MISMATCH ) {
    //    myprintf(" SCARD_E_PROTO_MISMATCH \n");
		return !neg & 1;
    }                                       
else if ( rv == SCARD_E_INVALID_VALUE ) {
     //   myprintf(" SCARD_E_INVALID_VALUE \n");
		return !neg & 1;
    }
    else if ( rv == SCARD_W_RESET_CARD ) {
      //  myprintf(" SCARD_E_RESET_CARD \n");
		return !neg & 1;
    }
    else if ( rv == SCARD_W_REMOVED_CARD ) {
       // myprintf(" SCARD_E_REMOVED_CARD \n");
		return !neg & 1;
    }
	return !neg & 1;
}
