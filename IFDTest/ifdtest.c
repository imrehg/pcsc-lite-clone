#include <stdio.h>
#include <winscard.h>
#include <configfile.h>
#include <stdlib.h>  
#include "commands.h"
#include <string.h>
#include <sys/utsname.h>
int readerlen,readerstate,atrlen;
char input;
int more_details=0;
int fails=0;
int p;
FILE *fp;
int iList[16];
int iReader;
SCARDHANDLE hCard;
unsigned char *data;
unsigned char *buffer;
SCARDCONTEXT hContext;
DWORD len=15;
UCHAR value[300];
struct ReaderContext *rContext;
SCARD_IO_REQUEST sSendPci, sRecvPci;
PSCARD_READERSTATE_A rgReaderStates[1];
DWORD dwReaderLen, dwState, dwProt, dwAtrLen;
DWORD dwSendLength, dwRecvLength, dwPref, dwReaders;
LPSTR pcReaders, mszReaders;
BYTE pbAtr[MAX_ATR_SIZE];
BYTE s[300], r[300];
LPCSTR mszGroups;
LONG rv;
DWORD state;
DWORD protocol;
char readerName[100];
char pcHost[50];
extern int testIBM(int ,char**);
extern int testSIEMENS(int ,char**);
extern int testSCHL(int ,char**);
extern int testAMMI(int ,char**);
extern int testGD(int ,char**);
extern int testBULL(int ,char**);

#define atrammi { 0x3b ,0x7e ,0x13 ,0x0 ,0x0 ,0x80 ,0x53 ,0xff ,0xff ,0xff ,0x62 ,0x0 ,0xff ,0x71 ,0xbf ,0x83 ,0x7 ,0x90 ,0x0 ,0x90,0 }
#define atrbull { 0x3f ,0x67 ,0x25 ,0x0 ,0x21 ,0x20 ,0x0 ,0xf ,0x68 ,0x90 ,0x00,0x90,0x00 }
#define atrgd  { 0x3b ,0xbf ,0x18 ,0x0 ,0x80 ,0x31 ,0x70 ,0x35 ,0x53 ,0x54 ,0x41 ,0x52 ,0x43 ,0x4f ,0x53 ,0x20 ,0x53 ,0x32 ,0x31 ,0x20 ,0x43 ,0x90 ,0x0 ,0x9b ,0x90,0x00 }
#define atribm { 0x3b ,0xef, 0x0, 0xff ,0x81 ,0x31 ,0x86 ,0x45 ,0x49 ,0x42 ,0x4d ,0x20 ,0x4d ,0x46 ,0x43 ,0x34 ,0x30 ,0x30 ,0x30 ,0x30 ,0x38 ,0x33 ,0x31 ,0x43 ,0x90,0x00 }
#define atrschl { 0x3b ,0xe2 ,0x0 ,0x0 ,0x40 ,0x20 ,0x99 ,0x1 ,0x90,0 }
#define atrsiemens { 0x3b ,0xef ,0x0 ,0x0 ,0x81 ,0x31 ,0x20 ,0x49 ,0x0 ,0x5c ,0x50 ,0x43 ,0x54 ,0x10 ,0x27 ,0xf8 ,0xd2 ,0x76 ,0x0 ,0x0 ,0x38 ,0x33 ,0x0 ,0x4d ,0x90,0x00 }

struct card cards[6] = { 
		{ 0,"AMMI",0,atrammi,19,testAMMI},
		{1, "BULL",0,atrbull,11,testBULL},
		{2,"G & D",0,atrgd,24,testGD},
		{3,"IBM",0,atribm,24,testIBM},
		{4,"SCHLUMBERGER",0,atrschl,8,testSCHL},
		{5,"SIEMENS NIXDORF",0,atrsiemens,24,testSIEMENS}
		};
void usage() {
	printf("ifdtest [-usage] [-debug] [-o filename ] \n");
	printf(" -usage : print the usage info\n");
	printf(" -debug : dump command APDUS and responses \n");
	printf(" -o filename : save output to filename \n");
	return;
	}

int checkATR(char *atr , int atrlen) {
	int i,j;
	if( atrlen == 0) return -1;
	for(i=0;i<6;i++)  {
		if( !cards[i].done ) {
			if( atrlen != cards[i].atrlen ) continue;
			for(j=0;j<atrlen;j++) {
				//myprintf("atr[%d] = %0x cards[%d].atr[%d] = %0x \n",j,atr[j],i,j,cards[i].atr[j]);
				if( (atr[j]&0xff) != cards[i].atr[j] ) break;
			}
			if( j== atrlen) {
				cards[i].done=1;
				return i;
			}
		}
	}
	return -1;
}
void printVersion(char * readerName ) {
    struct utsname *buf;
    int i;
    buf = malloc(sizeof(struct utsname));
    i=uname(buf);
    myprintf("Infineer IFDTest \n");
    myprintf("Reader Under test : %s \n", readerName);
    myprintf("Computer Name : %s \n", buf->nodename);
    myprintf("Version : %s \n",buf->release);
} 
int main(int argc,char** argv ) 
{
int i;
int alldone,card;
char *outfile=NULL;
if( argc > 1 ) {
for(i=1;i<argc;i++) {
	if( strcmp(argv[i],"-usage") == 0) {
		usage();
		return 0;
	}
	if( strcmp(argv[i],"-debug") == 0) 
		more_details=1;
	if(strcmp(argv[i],"-o")== 0) 
	  outfile=argv[i+1];
}
}
if( outfile != NULL) fp = fopen(outfile,"w");
else fp=NULL;
	rv = SCardEstablishContext( SCARD_SCOPE_GLOBAL, pcHost, NULL, &hContext );
	if ( rv != SCARD_S_SUCCESS ) {
			myprintf("ERROR :: Resource Manager not running \n");
			myprintf("Please start the resource manager and try again \n");
   		    return 1;
	} 
  rv = SCardSetTimeout(hContext,100);
  if ( rv != SCARD_S_SUCCESS ) {
    myprintf("ERROR :: Cannot Set timeout \n");
    myprintf("PCSC ERROR: %s\n", pcsc_stringify_error(rv));
    return -1;
  }
  rv = SCardGetStatusChange( hContext, INFINITE, 0, 0 );
 
  if ( rv != SCARD_S_SUCCESS ) {
    myprintf("PCSC ERROR: %s\n", pcsc_stringify_error(rv));
  }                                                      
  mszGroups = 0;
  rv = SCardListReaders( hContext, mszGroups, 0, &dwReaders );
 
  if ( rv != SCARD_S_SUCCESS ) {
    myprintf("PCSC ERROR: %s\n", pcsc_stringify_error(rv));
    myprintf("control: Error listing readers\n");
    return 1;        
 }
mszReaders = (char *)malloc(sizeof(char)*dwReaders);
rv = SCardListReaders( hContext, mszGroups, mszReaders, &dwReaders );
 
if ( rv != SCARD_S_SUCCESS ) {
    myprintf("PCSC ERROR: %s\n", pcsc_stringify_error(rv));
    myprintf("control: Error listing readers\n");
    return 1;
}
 /* Have to understand the multi-string here */
 p = 0;
  for ( i=0; i < dwReaders - 1; i++ ) {
    ++p;
    myprintf("Reader %02d: %s\n", p, &mszReaders[i]);
    iList[p] = i;
    while ( mszReaders[++i] != 0 );
  }
 
  do {
    myprintf("Enter the desired reader number : " );
    scanf("%d", &iReader); 
	myprintf("\n");
    if ( iReader > p || iReader <= 0 ) { 
	 myprintf("Invalid Value - try again\n");
    }
  } while ( iReader > p || iReader <= 0 );
	
  rgReaderStates[0] =
 (PSCARD_READERSTATE_A)
    malloc(sizeof(SCARD_READERSTATE_A));
	strcpy(readerName,&mszReaders[iList[iReader]]);
	printVersion(readerName);
  (rgReaderStates[0])->szReader       = strdup(readerName);
  (rgReaderStates[0])->dwCurrentState = SCARD_STATE_EMPTY; 
	myprintf("Do u want to run testA \n");
	read(0,&input,1);
	if( input == 'y') {
  		if(testA(argc,argv)) { 
			myprintf("testA Failed \n"); 	
			return -1;
		}
	}
	read(0,&input,1);
	myprintf("Do u want to run testB \n");
	read(0,&input,1);
	if( input == 'y') {
  		if(testB(argc,argv)) { 
			myprintf("testB Failed \n"); 	
			return -1;
		}
		myprintf("\n<<  Please remove smart card \n");
		(rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT; 	
		rv = SCardGetStatusChange( hContext,0xFFFFFFFF,rgReaderStates[0],1);
	}
	data = alloca(300);
	buffer = alloca(300);
	alldone=0;
	do {
	if( alldone == 6) break;
	myprintf("==========================\n");
	myprintf(" Part D: Smart Card Provider Test \n");
	myprintf("==========================\n");
	myprintf("Insert one of the following PC/SC test Cards \n");
	for(i=0;i<6;i++) {
		if(  !cards[i].done) {
			myprintf("*  %s\n",cards[i].name);
		}
	}
  		(rgReaderStates[0])->dwCurrentState = SCARD_STATE_EMPTY; 
	 	rv = SCardGetStatusChange( hContext,0xFFFFFFFF,rgReaderStates[0],1); 
		if( (rgReaderStates[0])->dwEventState & SCARD_STATE_PRESENT ) {
			rv = SCardConnect(hContext, readerName,
           	 SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0| SCARD_PROTOCOL_T1,
           	 &hCard, &dwPref); 
		  	if ( rv != SCARD_S_SUCCESS ) {
				continue;
  			}             
			readerlen=100;
			rv = SCardStatus(hCard,readerName,&readerlen,&readerstate,&protocol,r,&atrlen);
		  	if ( rv != SCARD_S_SUCCESS ) {
				continue;
  			}             
			card = checkATR(r,atrlen);
			if(card != -1) {
				myprintf("\nTesting card %s \n",cards[card].name); 
				(cards[card].func)(argc,argv);
				if( fails) myprintf("\n%s card Test Failed \n",cards[card].name);
				fails=0;
				alldone++;
			}
			else {
				myprintf("ATR doesn't match / Test already completed \n");
			}
				myprintf("\n<<  Please remove smart card \n");
				(rgReaderStates[0])->dwCurrentState = SCARD_STATE_PRESENT; 	
				rv = SCardGetStatusChange( hContext,0xFFFFFFFF,rgReaderStates[0],1);
		}
  	} while( alldone < 6  ) ;
}
