#define CLASS 0x00
#define SELECT(size,data,buffer,len) GetCommand(CLASS,0xa4,0,0,size,data,buffer,len)
#define READBIN(high,low,size,data,buffer,len) GetCommand(CLASS,0xb0,high,low,size,data,buffer,len)
#define WRITEBIN(high,low,size,data,buffer,len) GetCommand(CLASS,0xd0,high,low,size,data,buffer,len)
#define ERASEBIN(high,low,size,data,buffer,len) GetCommand(CLASS,0x0e,high,low,size,data,buffer,len)
#define VERIFY( keynumber,size,data,buffer,len) GetCommand(CLASS,0x2a,00,keynumber,size,data,buffer,len)
#define GETRESPONSE(size,len) GetCommand(CLASS,0xc0,00,00,size,data,buffer,len) 
#define UPDATEBIN(high,low,size,data,buffer,len) GetCommand(CLASS,0xd6,high,low,size,data,buffer,len)
#define DO_TRANSMIT(PROTOCOL,NEG)  if( more_details ) { \
myprintf("\n") ; \
for(j=0;j<dwSendLength ;j++) \
	myprintf("%0x ",buffer[j]); \
} \
dwRecvLength=300; \
	rv = SCardTransmit( hCard, PROTOCOL, buffer, dwSendLength, \
              &sRecvPci, r, &dwRecvLength ); \
	if( checkResult(rv,r,dwRecvLength,NEG) ) { \
		myprintf("Failed \n"); \
		fails=1; \
	} \
	else \
		myprintf("Passed \n"); \
if(more_details ) { \
myprintf("dwRecvlength = %0x \n",dwRecvLength ); \
for(j=0;j<dwRecvLength ;j++)  \
	myprintf("%0x ",r[j]); \
myprintf("\n"); \
}

extern void GetCommand(unsigned char class , unsigned char ins , unsigned char p1,unsigned char p2 ,unsigned char size , unsigned char * data , unsigned char *buffer,int length) ;
/*	if( r[dwRecvLength -2 ] == 0x61 ) \
	{ \
		dwSendLength=5; \
		GETRESPONSE(r[dwRecvLength-1],dwSendLength); \
		myprintf("result = %0x %0x \n", r[dwRecvLength-2],r[dwRecvLength-1]); \
		myprintf("doing get_response \n"); \
		rv = SCardTransmit(hCard,PROTOCOL,buffer,dwSendLength,&sRecvPci,r,&dwRecvLength); \
	}  */
/*
#define checkATR(atr) { \
	DWORD readerlen,readerstate,atrlen; \
	LPSTR readername = alloca(100); \
	readerlen=100; \
	if( argc == 2 ) \
		if( strcmp(argv[1],"-debug") == 0 ) more_details=1; \
	rv = SCardStatus(hCard,readername,&readerlen,&readerstate,&protocol,r,&atrlen); \
	if( rv != SCARD_S_SUCCESS ) { \
		myprintf("Remove card & Insert again %d ",rv); \
		if( rv == SCARD_E_INSUFFICIENT_BUFFER ) \
			myprintf("buffer error \n"); \
		if( rv == SCARD_E_READER_UNAVAILABLE ) { \
			myprintf("2 --------------------"); \
		} \
		return -1; \
	} \
	if( atrlen == 0) { \
		myprintf("ATR doesn't match \n"); \
		return -1; \
		} \
	for(j=0;j<atrlen;j++) \
		if( r[j] != atr[j] )  { \
			myprintf("Atr doesn't match r[%d] %0x atr[%d ] %0x \n",j,r[j],j,atr[j]); \
			return -1;  \
		}  \
	}
*/

	
extern int j,i;
extern int res;
extern int readerlen,readerstate,atrlen;
extern char input;
extern int more_details;
extern int p;
extern int iList[16];
extern int iReader;
extern SCARDHANDLE hCard;
extern unsigned char *data;
extern unsigned char *buffer;
extern SCARDCONTEXT hContext;
extern DWORD len;
extern UCHAR value[300];
extern struct ReaderContext *rContext;
extern SCARD_IO_REQUEST sSendPci, sRecvPci;
extern PSCARD_READERSTATE_A rgReaderStates[1];
extern DWORD dwReaderLen, dwState, dwProt, dwAtrLen;
extern DWORD dwSendLength, dwRecvLength, dwPref, dwReaders;
extern LPSTR pcReaders, mszReaders;
extern BYTE pbAtr[MAX_ATR_SIZE];
extern BYTE s[300], r[300];
extern LPCSTR mszGroups;
extern LONG rv;
extern DWORD state;
extern DWORD protocol;
extern char readerName[100];
extern char pcHost[50];
extern int fails;

typedef int ( *functions)(int , char **);
struct card {
    int id;
    char *name ;
    int done;
    unsigned char atr[100];
    int atrlen;
    functions func ;
	};
extern struct card cards[6];

extern FILE *fp;
