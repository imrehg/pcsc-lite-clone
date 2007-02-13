/***********************************************************
*        Title: pycsc.c
*       Author: Jean-Luc Giraud <jlgiraud@mac.com>
*     Compiler: gcc
*     Platform: Mac OS X, Linux and Windows2000
*      Purpose: Python class to communicate with smart cards
*      License: See file COPYING
*          $Id: pycsc.c,v 1.4 2004-06-21 00:54:29 giraud Exp $
*
*      Copyright (c) 2003 - Jean-Luc Giraud.
*
*      MODIFIED SEPT 2004 - Philippe C. Martin - SnakeCard, LLC (T=1)
*      MODIFIED APRIL 2006 - Philippe C. Martin - SnakeCard, LLC (SCardControl + PIN verify support)
************************************************************/

// workaround for using Borland C++ with Python compiled by Visual C++.
#if defined(SUPPRESS_DEBUG) && defined(_DEBUG)
#undef _DEBUG
#endif

#include <Python.h>

/* Include the header files */

#ifdef __APPLE__
#include <wintypes.h>
#include <PCSC/winscard.h>
#else
#ifdef WIN32
#include <windows.h>
#endif
#include <winscard.h>
#endif

#define PYCSC_PLATFORM_PCSCLITE "pcsc-lite"
#define PYCSC_PLATFORM_PCSCWINDOWS "pcsc-windows"


/* Internal tool */
static LONG getReaderList(SCARDCONTEXT hContext, LPSTR* pmszReaders, 
                          DWORD *pdwReaders);
static char *my_pcsc_stringify_error(LONG rv);

/* Windows does not provide the my_pcsc_stringify_error function */
/*******************************************************************
 **
 **  Function    : my_pcsc_stringify_error 
 **
 **  Description : Converts the long type resultant values into the 
 **                readable string format.
 **  Params (I)  : Return value in long format.
 **  Returns     : Return value in string converted format.
 **
 *******************************************************************/
char ErrorString[200];
char *my_pcsc_stringify_error(LONG rv)
{
	char strErrorCode[50];
	switch(rv) 
	{
#ifdef WIN32
		case ERROR_BROKEN_PIPE:
			strcpy(strErrorCode, "ERROR_BROKEN_PIPE");
			break;
		case SCARD_E_BAD_SEEK:
			strcpy(strErrorCode, "SCARD_E_BAD_SEEK");
			break;
		case SCARD_E_CERTIFICATE_UNAVAILABLE:
			strcpy(strErrorCode, "SCARD_E_CERTIFICATE_UNAVAILABLE");
			break;
		case SCARD_E_COMM_DATA_LOST:
			strcpy(strErrorCode, "SCARD_E_COMM_DATA_LOST");
			break;
		case SCARD_E_DIR_NOT_FOUND:
			strcpy(strErrorCode, "SCARD_E_DIR_NOT_FOUND");
			break;
		case SCARD_E_FILE_NOT_FOUND:
			strcpy(strErrorCode, "SCARD_E_FILE_NOT_FOUND");
			break;
		case SCARD_E_ICC_CREATEORDER:
			strcpy(strErrorCode, "SCARD_E_ICC_CREATEORDER");
			break;
		case SCARD_E_ICC_INSTALLATION:
			strcpy(strErrorCode, "SCARD_E_ICC_INSTALLATION");
			break;
		case SCARD_E_INVALID_CHV:
			strcpy(strErrorCode, "SCARD_E_INVALID_CHV");
			break;
		case SCARD_E_NO_SUCH_CERTIFICATE:
			strcpy(strErrorCode, "SCARD_E_NO_SUCH_CERTIFICATE");
			break;
		case SCARD_E_NOT_READY:
			strcpy(strErrorCode, "SCARD_E_NOT_READY");
			break;
		case SCARD_E_UNEXPECTED:
			strcpy(strErrorCode, "SCARD_E_UNEXPECTED");
			break;
		case SCARD_E_UNKNOWN_RES_MNG:
			strcpy(strErrorCode, "SCARD_E_UNKNOWN_RES_MNG");
			break;
		case SCARD_E_WRITE_TOO_MANY:
			strcpy(strErrorCode, "SCARD_E_WRITE_TOO_MANY");
			break;
		case SCARD_P_SHUTDOWN:
			strcpy(strErrorCode, "SCARD_P_SHUTDOWN");
			break;
		case SCARD_W_CANCELLED_BY_USER:
			strcpy(strErrorCode, "SCARD_W_CANCELLED_BY_USER");
			break;
		case SCARD_W_CHV_BLOCKED:
			strcpy(strErrorCode, "SCARD_W_CHV_BLOCKED");
			break;
		case SCARD_W_EOF:
			strcpy(strErrorCode, "SCARD_W_EOF");
			break;
		case SCARD_W_SECURITY_VIOLATION:
			strcpy(strErrorCode, "SCARD_W_SECURITY_VIOLATION");
			break;
		case SCARD_W_WRONG_CHV:
			strcpy(strErrorCode, "SCARD_W_WRONG_CHV");
			break;
#endif
		case SCARD_E_CANCELLED:
			strcpy(strErrorCode, "SCARD_E_CANCELLED");
			break;
		case SCARD_E_CANT_DISPOSE:
			strcpy(strErrorCode, "SCARD_E_CANT_DISPOSE");
			break;
		case SCARD_E_CARD_UNSUPPORTED:
			strcpy(strErrorCode, "SCARD_E_CARD_UNSUPPORTED");
			break;
		case SCARD_E_DUPLICATE_READER:
			strcpy(strErrorCode, "SCARD_E_DUPLICATE_READER");
			break;
		case SCARD_E_INSUFFICIENT_BUFFER:
			strcpy(strErrorCode, "SCARD_E_INSUFFICIENT_BUFFER");
			break;
		case SCARD_E_INVALID_ATR:
			strcpy(strErrorCode, "SCARD_E_INVALID_ATR");
			break;
		case SCARD_E_INVALID_HANDLE:
			strcpy(strErrorCode, "SCARD_E_INVALID_HANDLE");
			break;
		case SCARD_E_INVALID_PARAMETER:
			strcpy(strErrorCode, "SCARD_E_INVALID_PARAMETER");
			break;
		case SCARD_E_INVALID_TARGET:
			strcpy(strErrorCode, "SCARD_E_INVALID_TARGET");
			break;
		case SCARD_E_INVALID_VALUE:
			strcpy(strErrorCode, "SCARD_E_INVALID_VALUE");
			break;
		case SCARD_E_NO_MEMORY:
			strcpy(strErrorCode, "SCARD_E_NO_MEMORY");
			break;
		case SCARD_E_NO_READERS_AVAILABLE:
			strcpy(strErrorCode, "SCARD_E_NO_READERS_AVAILABLE");
			break;
		case SCARD_E_NO_SERVICE:
			strcpy(strErrorCode, "SCARD_E_NO_SERVICE");
			break;
		case SCARD_E_NO_SMARTCARD:
			strcpy(strErrorCode, "SCARD_E_NO_SMARTCARD");
			break;
		case SCARD_E_NOT_TRANSACTED:
			strcpy(strErrorCode, "SCARD_E_NOT_TRANSACTED");
			break;
		case SCARD_E_PCI_TOO_SMALL:
			strcpy(strErrorCode, "SCARD_E_PCI_TOO_SMALL");
			break;
		case SCARD_E_PROTO_MISMATCH:
			strcpy(strErrorCode, "SCARD_E_PROTO_MISMATCH");
			break;
		case SCARD_E_READER_UNAVAILABLE:
			strcpy(strErrorCode, "SCARD_E_READER_UNAVAILABLE");
			break;
		case SCARD_E_READER_UNSUPPORTED:
			strcpy(strErrorCode, "SCARD_E_READER_UNSUPPORTED");
			break;
		case SCARD_E_SERVICE_STOPPED:
			strcpy(strErrorCode, "SCARD_E_SERVICE_STOPPED");
			break;
		case SCARD_E_SHARING_VIOLATION:
			strcpy(strErrorCode, "SCARD_E_SHARING_VIOLATION");
			break;
		case SCARD_E_SYSTEM_CANCELLED:
			strcpy(strErrorCode, "SCARD_E_SYSTEM_CANCELLED");
			break;
		case SCARD_E_TIMEOUT:
			strcpy(strErrorCode, "SCARD_E_TIMEOUT");
			break;
		case SCARD_E_UNKNOWN_CARD:
			strcpy(strErrorCode, "SCARD_E_UNKNOWN_CARD");
			break;
		case SCARD_E_UNKNOWN_READER:
			strcpy(strErrorCode, "SCARD_E_UNKNOWN_READER");
			break;
		case SCARD_E_UNSUPPORTED_FEATURE:
			strcpy(strErrorCode, "SCARD_E_UNSUPPORTED_FEATURE");
			break;
		case SCARD_F_COMM_ERROR:
			strcpy(strErrorCode, "SCARD_F_COMM_ERROR");
			break;
		case SCARD_F_INTERNAL_ERROR:
			strcpy(strErrorCode, "SCARD_F_INTERNAL_ERROR");
			break;
		case SCARD_F_UNKNOWN_ERROR:
			strcpy(strErrorCode, "SCARD_F_UNKNOWN_ERROR");
			break;
		case SCARD_F_WAITED_TOO_LONG:
			strcpy(strErrorCode, "SCARD_F_WAITED_TOO_LONG");
			break;
		case SCARD_S_SUCCESS:
			strcpy(strErrorCode, "SCARD_S_SUCCESS");
			break;
		case SCARD_W_REMOVED_CARD:
			strcpy(strErrorCode, "SCARD_W_REMOVED_CARD");
			break;
		case SCARD_W_RESET_CARD:
			strcpy(strErrorCode, "SCARD_W_RESET_CARD");
			break;
		case SCARD_W_UNPOWERED_CARD:
			strcpy(strErrorCode, "SCARD_W_UNPOWERED_CARD");
			break;
		case SCARD_W_UNRESPONSIVE_CARD:
			strcpy(strErrorCode, "SCARD_W_UNRESPONSIVE_CARD");
			break;
		case SCARD_W_UNSUPPORTED_CARD:
			strcpy(strErrorCode, "SCARD_W_UNSUPPORTED_CARD");
			break;
		default:
			strcpy(strErrorCode, "Unknown");
	}
#ifdef WIN32
    sprintf(ErrorString, "%s (0x%08lX)", strErrorCode, rv);
#else
    sprintf(ErrorString, "%s %s (0x%08lX)", pcsc_stringify_error(rv),
		strErrorCode, rv);
#endif
    return ErrorString;
}

/* define MAX_ATR_SIZE for all platforms */
#ifndef MAX_ATR_SIZE
#define MAX_ATR_SIZE 300
#endif


/***********************************************************
 * pycsc object structure
 ***********************************************************/
typedef struct 
{
  PyObject_HEAD
  SCARDHANDLE  hCard;
  SCARDCONTEXT hContext; 
  DWORD sMode;
  DWORD dwProtocol;
} pycscobject;

/* Exception used by this object  */
static PyObject *PycscException;

/* Forward declaration of type descriptor */
staticforward PyTypeObject PycscType;

/***********************************************************
 * Intance methods:
 *
 * reconnect()
 * disconnect()
 * status()
 * control()
 * transmit()
 * cancel()
 * beginTransaction()
 * endTransaction()
 * cancelTransaction()
 *
 ***********************************************************/

 /******************************************************************
  **
  **  Function    : pycsc_reconnect
  **
  **  Description : This function reestablishes a connection to a 
  **				reader that was previously connected to. In a 
  **				multi application environment it is possible 
  **				for an application to reset the card in shared 
  **				mode.
  **
  ******************************************************************/
static PyObject * pycsc_reconnect( PyObject *self,  PyObject *args,  PyObject *keywds)
{
  pycscobject *object = (pycscobject *)self;
  LONG rv;
  DWORD dwPreferredProtocol = SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1;
  DWORD dwInitialization    = SCARD_LEAVE_CARD;
  DWORD pdwActiveProtocol;

  static char *kwlist[] = {"smode", "protocol", "init", NULL};

  object->sMode = SCARD_SHARE_SHARED; /* default value */

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "|lll", kwlist,
                   &object->sMode, &dwPreferredProtocol,
                   &dwInitialization))
    return NULL;

#ifdef DEBUG
  printf("sMode = %ld\n",object->sMode);
  printf("pProt = %ld\n",dwPreferredProtocol);
  printf("init = %ld\n",dwInitialization);
  printf("IN RECONNECT\n");
  printf("object->sMode %d\n",object->sMode);
  printf("object->dwPreferredProtocol %d\n",dwPreferredProtocol);
  printf("object->dwInitialization %d\n",dwInitialization);
#endif


  rv = SCardReconnect(object->hCard, object->sMode, dwPreferredProtocol,
              dwInitialization, &pdwActiveProtocol);

  if ( rv != SCARD_S_SUCCESS )
    {
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
    }

  Py_INCREF(Py_None);
  return Py_None;
}

/******************************************************************
 **
 **  Function    : pycsc_disconnect
 **
 **  Description : Closes connection to the smart card reader.
 **
 ******************************************************************/

static PyObject * pycsc_disconnect(PyObject *self, PyObject * args)
{
  pycscobject *object = (pycscobject *)self;
  DWORD dwDisposition = SCARD_LEAVE_CARD;
  LONG  rv;

  if (!PyArg_ParseTuple(args,"|l", &dwDisposition))
    return NULL;

  rv = SCardDisconnect(object->hCard, dwDisposition);

  if ( rv != SCARD_S_SUCCESS )
    {
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
    }
#ifdef DEBUG
  printf("pycsc_disconnect : OK\n");
#endif

  Py_INCREF(Py_None);
  return Py_None;
}

/******************************************************************
 **
 **  Function    : pycsc_status
 **
 **  Description : Retrieves status of the card. The extracted 
 **				   fields are ReaderName, State, Protocol, ATR.
 **
 ******************************************************************/
static PyObject * pycsc_status(PyObject *self, PyObject * args)
{
  pycscobject *object = (pycscobject *)self;
  BYTE  pbAtr[MAX_ATR_SIZE];
  DWORD dwAtrLen, dwProt=0, dwState=0;
  DWORD dwReaderLen;
  LPSTR pcReaders;
  LONG  rv;
  PyObject *ret_value;

  dwReaderLen = 10000;
  dwAtrLen = 0;
  /* Dry run to get the length of the reader name */ 
  rv = SCardStatus( object->hCard, (LPSTR) NULL, &dwReaderLen, 
            &dwState, &dwProt, NULL, &dwAtrLen );

  if ( rv != SCARD_S_SUCCESS )
    {
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
    }

#ifdef DEBUG
  printf("SCardStatus dry run returned dwReaderLen= %ld, dwAtrLen= %ld\n",
         dwReaderLen, dwAtrLen);
#endif
  pcReaders = (char *)PyMem_Malloc(sizeof(char)*dwReaderLen);
  if (!pcReaders)
    {
      return PyErr_NoMemory();
    }

  /* Get values */
  dwAtrLen = sizeof(pbAtr);
  rv = SCardStatus( object->hCard, pcReaders, &dwReaderLen,
            &dwState, &dwProt, pbAtr, &dwAtrLen );

  if ( rv != SCARD_S_SUCCESS )
    {
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      PyMem_Free((void *)pcReaders);
      return NULL;
    }

#ifdef DEBUG
  printf("pycsc_status : OK up to build value\n");
#endif
  ret_value = Py_BuildValue("{s:s, s:i, s:i, s:s#}",
                "ReaderName", pcReaders,
                "State", dwState,
                "Protocol", dwProt,
                "ATR", pbAtr, dwAtrLen);

#ifdef DEBUG
  printf("pycsc_status : OK\n");
#endif
  PyMem_Free((void *)pcReaders);
  return ret_value;
}

/******************************************************************
 **
 **  Function    : pycsc_control
 **
 **  Description : Should invoke SCardcontrol with necessary 
 **				   parameters.This function sends a command directly
 **				   to the IFD Handler to be processed by the reader.
 **				   This is useful for creating client side reader 
 **				   drivers for functions like PIN pads, biometrics, 
 **				   or other extensions to the normal smart card reader 
 **				   that are not normally handled by PC/SC.
 **
 ******************************************************************/
static PyObject * pycsc_control(PyObject *self, PyObject * args)
{
	pycscobject *object = (pycscobject *)self;
	DWORD dwControlCode;
	unsigned char * inBuffer;
	unsigned long inBufferLen, outBufferLen;
	unsigned char outBuffer[0x800];
	LONG  rv;

	if (!PyArg_ParseTuple(args,"ks#", &dwControlCode, &inBuffer, &inBufferLen))
	{
		PyErr_SetString(PycscException, "pycsc_control: parameters parsing failed");
		return NULL;
	}
#ifdef __APPLE__
        outBufferLen = sizeof(outBuffer);
	rv = SCardControl(object->hCard, inBuffer, inBufferLen, outBuffer, 
                          &outBufferLen);
#else
        rv = SCardControl(object->hCard, dwControlCode, inBuffer, inBufferLen, 
                          outBuffer, sizeof(outBuffer), &outBufferLen);
#endif
	if ( rv != SCARD_S_SUCCESS )
	{
		PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
		return NULL;
	}

	return Py_BuildValue("s#", outBuffer, outBufferLen);
}

static PyObject * pycsc_getAttrib(PyObject *self, PyObject * args)
{
//+ NEED TO CHECK IF IT REALLY DOES NOT EXIST ON PCSC/LITE
#ifdef WINDOWS
	pycscobject *object = (pycscobject *)self;
	DWORD attrId;
	LPBYTE attrValue = NULL;
	DWORD attrValueLen = SCARD_AUTOALLOCATE;
	LONG  rv;
	PyObject * result;

	// read parameter (attrId)
	if (!PyArg_ParseTuple(args,"k", &attrId))
		return NULL;

	// retrieve attribute value
	rv = SCardGetAttrib(object->hCard, attrId, (LPBYTE) &attrValue, &attrValueLen);
	if ( rv != SCARD_S_SUCCESS )
	{
		PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
		return NULL;
	}

	// create result object of attrValue
	result = Py_BuildValue("s#", attrValue, attrValueLen);

	SCardFreeMemory(object->hContext, attrValue);
	return result;
#else
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
#endif
}

static PyObject * pycsc_setAttrib(PyObject *self, PyObject * args)
{
#ifdef WINDOWS
	pycscobject *object = (pycscobject *)self;
	DWORD attrId;
	DWORD attrValueLen;
	LPCBYTE attrValue;
	LONG  rv;

	if (!PyArg_ParseTuple(args,"ks#", &attrId, &attrValue, &attrValueLen))
		return NULL;

	rv = SCardSetAttrib(object->hCard, attrId, attrValue, attrValueLen);
	if ( rv != SCARD_S_SUCCESS )
	{
		PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
#else
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
#endif
}


/******************************************************************
 **
 **  Function    : pycsc_control
 **
 **  Description : Retrieve SCARD_PCI_XX address from
 **				   SCARD_PROTOCOL_XX constant 
 **
 ******************************************************************/
/* Retrieve SCARD_PCI_XX address from SCARD_PROTOCOL_XX constant */
static SCARD_IO_REQUEST *protocoltoPCI(DWORD dwProtocol)
{
  switch (dwProtocol)
  {
    case SCARD_PROTOCOL_T0:
      return (SCARD_IO_REQUEST *)SCARD_PCI_T0;
    case SCARD_PROTOCOL_T1:
      return (SCARD_IO_REQUEST *)SCARD_PCI_T1;
    case SCARD_PROTOCOL_RAW:
      return (SCARD_IO_REQUEST *)SCARD_PCI_RAW;
    default:
      PyErr_SetString(PycscException, "Unknow value of io request");
      return NULL;
  }
}

/******************************************************************
 **
 **  Function    : pycsc_transmit
 **
 **  Description : Transmits APDU commands to the smart card. The 
 **				   responds to the APDU.
 **
 ******************************************************************/
static PyObject * pycsc_transmit(PyObject *self, PyObject * args, 
                                 PyObject *keywds)
{  
  pycscobject *object = (pycscobject *)self;
  LONG rv;
  unsigned long len;
  unsigned char *sendBuffer;
  DWORD bSendPci;
  SCARD_IO_REQUEST *pioSendPci;
  BYTE  resparray[1024];
  DWORD resplength = sizeof(resparray);
  static char *kwlist[] = {"com","sendPCI", NULL};

  /* Default values */
  bSendPci = object->dwProtocol;

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s#|l", kwlist,
                                   &sendBuffer, &len, &bSendPci))
    return NULL;

  pioSendPci = protocoltoPCI(bSendPci);
  if (pioSendPci == NULL)
    return NULL;

  rv = SCardTransmit( object->hCard, pioSendPci, sendBuffer, len, 
                     NULL, resparray, &resplength);
  if ( rv != SCARD_S_SUCCESS )
  {
    PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
    return NULL;
  }

  return Py_BuildValue("s#", resparray, resplength);
}

/******************************************************************
 **
 **  Function    : pycsc_cancel
 **
 **  Description : Should invoke SCardCancel() with necessary 
 **				   parameters. This function cancels all pending
 **				   blocking requests.
 **
 ******************************************************************/

static PyObject * pycsc_cancel(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}

/******************************************************************
 **
 **  Function    : pycsc_beginTransaction
 **
 **  Description : Should invoke SCardBeginTransaction with necessary
 **				   parameters.This function establishes a temporary 
 **				   exclusive access mode for doing a series of commands 
 **                or transaction. You might want to use this when you 
 **				   are selecting a few files and then writing a large 
 **				   file so you can make sure that another application 
 **				   will not change the current file. If another 
 **				   application has a lock on this reader or this 
 **			       application is in SCARD_SHARE_EXCLUSIVE there will
 **				   be no action taken.
 **
 ******************************************************************/

static PyObject * pycsc_beginTransaction(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}


/******************************************************************
 **
 **  Function    : pycsc_endTransaction
 **
 **  Description : Should invoke SCardEndTransaction with necessary
 **				   parameters. This function ends a previously begun
 **			       transaction. The calling application must be the
 **				   owner of the previously begun transaction or an 
 **			       error will occur. 
 **
 ******************************************************************/

static PyObject * pycsc_endTransaction(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}


/******************************************************************
 **
 **  Function    : pycsc_cancelTransaction
 **
 **  Description : Should invoke SCardCancel with necessary 
 **				   parameters. This function cancels all pending 
 **				   blocking requests.
 **
 ******************************************************************/

static PyObject * pycsc_cancelTransaction(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}


/* Declaration of methods */
static struct PyMethodDef pycsc_methods[] =
{
	{"reconnect",(PyCFunction)pycsc_reconnect,     METH_VARARGS|METH_KEYWORDS,
		"reconnect(smode, protocol, init) -> None." },
	{"disconnect",        pycsc_disconnect,        METH_VARARGS,
		"disconnect() -> None." },
	{"status",            pycsc_status,            METH_VARARGS,
		"status() -> {\"ReaderName\":str, \"Protocol\":int, \"State\":int, \"ATR\":str}"},
	{"control",           pycsc_control,           METH_VARARGS,
		"control(controlCode, inBuffer) -> outBuffer.\n\
Gets direct control of the reader after connection to card/reader is established.\
inBuffer and outBuffer are strings."},
	{"getAttrib",         pycsc_getAttrib,         METH_VARARGS,
		"getAttrib(attrId) -> attrValue"},
	{"setAttrib",         pycsc_setAttrib,         METH_VARARGS,
		"getAttrib(attrId, attrValue)"},
	{"transmit",(PyCFunction)pycsc_transmit,       METH_VARARGS|METH_KEYWORDS,
		"transmit(com, sendPCI) -> resp.\n\
Sends a APDU (the string in 'com') to the card. By specifing sendPCI \
different protocol can be used"},
	{"cancel",            pycsc_cancel,            METH_VARARGS,
		"not implemented yet"},
	{"beginTransaction",  pycsc_beginTransaction,  METH_VARARGS,
		"not implemented yet"},
	{"endTransaction",    pycsc_endTransaction,    METH_VARARGS,
		"not implemented yet"},
	{"cancelTransaction", pycsc_cancelTransaction, METH_VARARGS,
		"not implemented yet"},
	{NULL,                NULL}
};


/***********************************************************
 * Type methods
 *
 * dealloc() (Destructor)
 * getattr()
 * repr()
 * 
 ***********************************************************/
static void pycsc_dealloc(PyObject *self)
{
  pycscobject *object = (pycscobject *)self;
#ifdef DEBUG
  SCARDCONTEXT  hContext;
  SCARDHANDLE   hCard;
  hContext = ((pycscobject *)self)->hContext;
  hCard    = ((pycscobject *)self)->hCard;
#endif


  //+ Disconnect leaving card untouched, any better idea?
  SCardDisconnect( object->hCard, SCARD_LEAVE_CARD );
  SCardReleaseContext( object->hContext );
  PyObject_Del(object);
#ifdef DEBUG
  printf("pycsc_dealloc successful for hContext = %08X, hCard = %08X\n",
         hContext, hCard);
#endif

}

static PyObject * pycsc_getattr(PyObject *self, char* name)
{
  pycscobject *object = (pycscobject *)self;

  return Py_FindMethod(pycsc_methods,  (PyObject *) object, name);
}

static PyObject * pycsc_repr(PyObject *self)
{
  pycscobject *object = (pycscobject *)self;
  char buffer[1024];

  snprintf(buffer, sizeof(buffer), 
       "< pycsc, hContext = 0x%08lX, hCard = 0x%08lX at %lx >",
       object->hContext, object->hCard, (LONG) object);
  return PyString_FromString(buffer);
}

statichere PyTypeObject PycscType =
{
    PyObject_HEAD_INIT(NULL)
    0,                          /* ob_size */
    "pycsc",                    /* tp_name */
    sizeof(pycscobject),        /* tp_basicsize */
    0,                          /* tp_itemsize */
    /* methods */
    pycsc_dealloc,              /* tp_dealloc */
    0,                          /* tp_print */
    (getattrfunc) pycsc_getattr,/* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_compare */
    (reprfunc) pycsc_repr,      /* tp_repr */
    0,                          /* tp_as_number */
    0,                          /* tp_as_sequence */
    0,                          /* tp_as_mapping */
    (hashfunc) 0,               /* tp_hash */
    0,                          /* tp_call */
    (reprfunc) 0,               /* tp_str */
};




/***********************************************************
 * Class methods
 *
 * pycsc()
 * listReader()
 * listReaderGroups()
 * getStatusChange()
 * 
 ***********************************************************/


/* This is the "creator" for a new pycsc object */

/******************************************************************
 **
 **  Function    : pycscobject_pycsc
 **
 **  Description : Creates new pycsc object and establishes the 
 **				   connection to the Smart Card Reader.
 **
 ******************************************************************/

static PyObject * pycscobject_pycsc(PyObject *self, PyObject * args, PyObject *keywds)
{
  /* No reader name in args, connect to the first reader */
  LPSTR mszReaders = NULL;
  LPSTR szRequestedReader = "";
  DWORD dwReaders;
  DWORD dwMode = SCARD_SHARE_SHARED;
  DWORD eProtocol;   /* effective protocol */
  DWORD dwPreferredProtocol = SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1;
  SCARDCONTEXT  hContext;
  SCARDHANDLE   hCard;
  LONG rv;
  pycscobject *newself;
  static char *kwlist[] = {"reader", "mode", "protocol", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "|s#ll", kwlist,
                                   &szRequestedReader, &dwReaders,
                                   &dwMode,
                                   &dwPreferredProtocol))
    return NULL;

#ifdef DEBUG
  printf("pycsc called with: ");
  if (szRequestedReader)
    printf("Reader name: %s\n", szRequestedReader);
  else
    printf("Reader name not specified\n");
  printf("Mode: %ld\n", dwMode);
  printf("Preferred protocol: %ld\n", dwPreferredProtocol);
#endif  


  /* Initialise context */
  rv = SCardEstablishContext( SCARD_SCOPE_SYSTEM, NULL, NULL,
                  &hContext);
  if ( rv != SCARD_S_SUCCESS )
  {
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
  }

#ifdef DEBUG
  printf("SCardEstablishContext successful\n");
#endif
  if (strlen(szRequestedReader) > 0)
  {
    // malloc a buffer to copy the reader name to have the same behiaviour
    // as when selecting the first one
    mszReaders = (char *)PyMem_Malloc(sizeof(char)*
                                      (strlen(szRequestedReader)+1));

    if (!mszReaders)
    {
      SCardReleaseContext( hContext );
      return PyErr_NoMemory();
    }
    strncpy(mszReaders, szRequestedReader, strlen(szRequestedReader)+1);
  }
  else
  {
    // No reader specified, need to locate the first one
    rv = getReaderList(hContext, &mszReaders,  &dwReaders);

    if ( rv != SCARD_S_SUCCESS )
    {
      SCardReleaseContext( hContext );
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
    }
  }
  if (*mszReaders == '\0')
  {
    SCardReleaseContext( hContext );
    PyErr_SetString(PycscException, "No reader connected");
    return NULL;    
  }
  /* Connect */
  rv = SCardConnect(hContext, mszReaders, dwMode, 
            dwPreferredProtocol, &hCard, &eProtocol);
  /* Free the memory now */
  PyMem_Free((void *)mszReaders);

  if ( rv != SCARD_S_SUCCESS )
  {
    SCardReleaseContext( hContext );
    PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
    return NULL;
  }

  /* Now that everything worked, alloc new object */
  /* replace self from class with the new instance */
  newself = PyObject_New(pycscobject, & PycscType);
  if (newself == NULL)
  {
    SCardReleaseContext( hContext );
    PyErr_SetString(PycscException, "Could not allocate new object");
    return NULL;
  }
  newself->hContext   = hContext;
  newself->hCard      = hCard;
  newself->sMode      = dwMode;
  newself->dwProtocol = eProtocol;

#ifdef DEBUG
  printf("Active protocol: 0xld\n", eProtocol);
  printf("pycsc_pycsc: OK with  hContext = 0x%08X, hCard = 0x%08X\n",
          hContext, hCard);
#endif

  return (PyObject *) newself;
}


/******************************************************************
 **
 **  Function    : pycscobject_listReader
 **
 **  Description : This function returns a list of currently 
 **				   available readers on the system.
 **
 ******************************************************************/

static PyObject * pycscobject_listReader(PyObject *self, PyObject * args)
{
  SCARDCONTEXT  hContext;
  LPSTR mszReaders = NULL;
  LPSTR mszReadersScan;
  DWORD dwReaders;
  LONG rv;

  PyObject *ret_value;

  rv = SCardEstablishContext( SCARD_SCOPE_SYSTEM, NULL, NULL,
                              &hContext);
  if ( rv != SCARD_S_SUCCESS )
  {
    PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
    return NULL;
  }
  rv = getReaderList(hContext, &mszReaders,  &dwReaders);

  if ( rv != SCARD_S_SUCCESS )
  {
    PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
    return NULL;
  }
  // Build output from mszReaders
  ret_value = Py_BuildValue("[]");
  if (!ret_value)
  {
    PyErr_SetString(PycscException, "Could not create list");
    SCardReleaseContext(hContext);  
    PyMem_Free((void *)mszReaders);
    return NULL;
  }
  mszReadersScan = mszReaders;
  while (*mszReadersScan != '\0')
  {
    //+ Does that free memomry properly in case of error with the append
    //+ probably Py_BuildValue("s", mszReadersScan) needs a DECREF
    //+ and so does ret_value
    if (PyList_Append(ret_value, Py_BuildValue("s", mszReadersScan)))
    {
      PyErr_SetString(PycscException, "Could not append reader name");
      SCardReleaseContext(hContext);  
      PyMem_Free((void *)mszReaders);
      return NULL;
    }      
    mszReadersScan += strlen(mszReadersScan)+1;
  }
  SCardReleaseContext(hContext);  
  PyMem_Free((void *)mszReaders);
  return ret_value;

}

/******************************************************************
 **
 **  Function    : pycscobject_listReaderGroups
 **
 **  Description : This function returns a list of currently 
 **				   available reader groups on the system.
 **
 ******************************************************************/

static PyObject * pycscobject_listReaderGroups(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}

static PyObject * pycscobject_getStatusChange(PyObject *self, PyObject * args,  PyObject *keywds)
{
  SCARDCONTEXT  hContext;
  PyObject *ret_value = NULL;
  SCARD_READERSTATE *pstReaderStates;
  DWORD timeout = INFINITE;
  PyObject *pyReaderStates = NULL;
  PyObject *tmpSeq;
  int readerstatesLength = 0;
  int i;
  LONG rv;

  static char *kwlist[] = {"Timeout", "ReaderStates", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "|lO", kwlist,
                   &timeout, &pyReaderStates))
  {
      PyErr_SetString(PycscException, "Error parsing arguments");
      return NULL;
  }
  // Create an empty sequence in case pyReaderStates was not given as arg
  tmpSeq = Py_BuildValue("[]");
  if (pyReaderStates==NULL)
  {
      pyReaderStates = tmpSeq;
  }
  // Parse the readerstate argument
  pyReaderStates = PySequence_Fast(pyReaderStates, "pycsc: getStatusChange: Expecting sequence argument for readerstates");
  Py_DECREF(tmpSeq);
  if ( ! pyReaderStates )
  {
      PyErr_SetString(PycscException, "Error in call to PySequence_Fast");
      return NULL;
  }
  readerstatesLength = PySequence_Fast_GET_SIZE(pyReaderStates);
  pstReaderStates = PyMem_Malloc(sizeof(SCARD_READERSTATE)*readerstatesLength);
  if (!pstReaderStates)
  {
      Py_DECREF(pyReaderStates);
      return PyErr_NoMemory();
  }
 
  // Iterate on the sequence. Each element should be a mapping reflecting the content of the struct
  for (i=0; i<readerstatesLength; i++)
  {
      PyObject *pyReaderState;
      pyReaderState = PySequence_Fast_GET_ITEM(pyReaderStates, i);
      if ( (!pyReaderState) || (! PyMapping_Check(pyReaderState)) )
      {
          // Release memory
          for (;i>0; i--)
          {
              PyMem_Free((void *)pstReaderStates[i-1].szReader);
          }
          PyMem_Free((void *)pstReaderStates);
          Py_DECREF(pyReaderStates);
          PyErr_SetString(PycscException, "pycsc: getStatusChange: Expecting mapping argument for each readerstate");
          return NULL;
      }
      // Parse the content of the mapping to set the structure
      // The keys of the mapping are:
      // "reader" for the reader name
      // "currentState" for the set of state information to watch for  
      // Missing value values will be replaced by default values ("" for reader, SCARD_STATE_EMPTY for currentState)
      pstReaderStates[0].szReader = "";
      pstReaderStates[0].dwCurrentState = SCARD_STATE_EMPTY;
      // First deal with dwCurrentState as the szReader free loop on error will be the same as below
      if (PyMapping_HasKeyString(pyReaderState, "CurrentState"))
      {
          PyObject *pycurrentState = NULL;
          pycurrentState = PyMapping_GetItemString(pyReaderState, "CurrentState");
          if (!PyInt_Check(pycurrentState))
          {
              for (;i>0; i--)
              {
                  PyMem_Free((void *)pstReaderStates[i-1].szReader);
              }
              PyMem_Free((void *)pstReaderStates);
              Py_DECREF(pyReaderStates);
              PyErr_SetString(PycscException, "pycsc: getStatusChange: Expecting \"CurrentState\" key to reference an integer");
              return NULL;              
          }
          pstReaderStates[i].dwCurrentState = PyInt_AsLong(pycurrentState);
      }
      if (PyMapping_HasKeyString(pyReaderState, "Reader"))
      {
          PyObject *pyReader = NULL;
          char *pcReaderName;
          int length;
          pyReader = PyMapping_GetItemString(pyReaderState, "Reader");
          if (!PyString_Check(pyReader))
          {
              for (;i>0; i--)
              {
                  PyMem_Free((void *)pstReaderStates[i-1].szReader);
              }
              PyMem_Free((void *)pstReaderStates);
              Py_DECREF(pyReaderStates);
              PyErr_SetString(PycscException, "pycsc: getStatusChange: Expecting \"Reader\" key to reference a string");
              return NULL;
          } 
          // Make a copy of the string.
          length = sizeof(char)*(strlen(PyString_AsString(pyReader))+1);
          pcReaderName = PyMem_Malloc(length);
          if (!pcReaderName)
          {
              for (;i>0; i--)
              {
                  PyMem_Free((void *)pstReaderStates[i-1].szReader);
              }
              PyMem_Free((void *)pstReaderStates);
              Py_DECREF(pyReaderStates);              
              PyErr_SetString(PycscException, "pycsc: getStatusChange: Memory allocation error");
              return NULL;
          }
          strncpy(pcReaderName, PyString_AsString(pyReader), length);
          pcReaderName[length-1] = '\0';
          pstReaderStates[i].szReader = pcReaderName;
          pstReaderStates[i].pvUserData = NULL;
      }
  }
  // Input is not required anymore.
  Py_DECREF(pyReaderStates);

  // Connect to pcsc.
  rv = SCardEstablishContext( SCARD_SCOPE_SYSTEM, NULL, NULL,
                              &hContext);
  if ( rv != SCARD_S_SUCCESS )
  {
      for (i=0;i<readerstatesLength; i++)
      {
          PyMem_Free((void *)pstReaderStates[i].szReader);
      }
      PyMem_Free((void *)pstReaderStates);
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
  }


  rv = SCardGetStatusChange(hContext, timeout, pstReaderStates,
                            readerstatesLength);
  SCardReleaseContext(hContext);  
  if ( rv != SCARD_S_SUCCESS )
  {
      for (i=0;i<readerstatesLength; i++)
      {
          PyMem_Free((void *)pstReaderStates[i].szReader);
      }
      PyMem_Free((void *)pstReaderStates);
      PyErr_SetString(PycscException, my_pcsc_stringify_error(rv));
      return NULL;
  }
  
  // Construct the response sequence of mappings
  ret_value = Py_BuildValue("[]");
  if (!ret_value)
  {
      for (i=0;i<readerstatesLength; i++)
      {
          PyMem_Free((void *)pstReaderStates[i].szReader);
      }
      PyMem_Free((void *)pstReaderStates);
      PyErr_SetString(PycscException, "Could not create list");
      return NULL;
  }
  for (i=0;i<readerstatesLength; i++)
  {
      PyObject *pyreaderstate;
      pyreaderstate = Py_BuildValue("{s:s, s:s, s:i, s:i, s:s#}",
                                    "Reader", pstReaderStates[i].szReader,
                                    "UserData", "",
                                    "CurrentState", pstReaderStates[i].dwCurrentState,
                                    "EventState", pstReaderStates[i].dwEventState,
                                    "Atr", pstReaderStates[i].rgbAtr, pstReaderStates[i].cbAtr);
      if (!pyreaderstate)
      {
          Py_DECREF(ret_value);
          // Up to the value of i (non inclusive), the memory has already been released
          for (;i<readerstatesLength; i++)
          {
              PyMem_Free((void *)pstReaderStates[i].szReader);
          }
          PyMem_Free((void *)pstReaderStates);
          PyErr_SetString(PycscException, "Could not create dictionnary");
          return NULL;
      }
      if (PyList_Append(ret_value, Py_BuildValue("O", pyreaderstate)))
      {
          PyErr_SetString(PycscException, "Could not append dictionnary to sequence");
          Py_DECREF(ret_value);
          Py_DECREF(pyreaderstate);
          // Up to the value of i (non inclusive), the memory has already been released
          for (;i<readerstatesLength; i++)
          {
              PyMem_Free((void *)pstReaderStates[i].szReader);
          }
          PyMem_Free((void *)pstReaderStates);
          PyErr_SetString(PycscException, "Could not create dictionnary");
          return NULL;
      }      
      // Release memory as soon as we don't need it anymore
      PyMem_Free((void *)pstReaderStates[i].szReader);
  }

  
  // Release memory
  PyMem_Free((void *)pstReaderStates);

  return ret_value;
}


/* Declaration of methods */
static struct PyMethodDef pycsctype_methods[] =
{
	{"pycsc",(PyCFunction)pycscobject_pycsc,             METH_VARARGS|METH_KEYWORDS},
	{"listReader",        pycscobject_listReader,        METH_VARARGS},
	{"listReaderGroups",  pycscobject_listReaderGroups,  METH_VARARGS},
	{"getStatusChange",(PyCFunction)   pycscobject_getStatusChange,   METH_VARARGS|METH_KEYWORDS},
	{(char*)NULL,         (PyCFunction)NULL,             (int)NULL}
};


/* Module initialisation */
void initpycsc(void)
{
  PyObject *m;

	/* patch object type for building dll on windows... */
	PycscType.ob_type = &PyType_Type;
	m = Py_InitModule("pycsc", pycsctype_methods);

	//+ Add error code and constants definitions
	PyModule_AddIntConstant(m, "SCARD_LEAVE_CARD", SCARD_LEAVE_CARD);
	PyModule_AddIntConstant(m, "SCARD_LEAVE_CARD", SCARD_LEAVE_CARD);
	PyModule_AddIntConstant(m, "SCARD_RESET_CARD", SCARD_RESET_CARD);
	PyModule_AddIntConstant(m, "SCARD_SHARE_SHARED", SCARD_SHARE_SHARED);
	PyModule_AddIntConstant(m, "SCARD_SHARE_EXCLUSIVE", SCARD_SHARE_EXCLUSIVE);
	PyModule_AddIntConstant(m, "SCARD_SHARE_DIRECT", SCARD_SHARE_DIRECT);

	PyModule_AddIntConstant(m, "SCARD_PROTOCOL_T0", SCARD_PROTOCOL_T0);
	PyModule_AddIntConstant(m, "SCARD_PROTOCOL_T1", SCARD_PROTOCOL_T1);
	PyModule_AddIntConstant(m, "SCARD_PROTOCOL_RAW", SCARD_PROTOCOL_RAW);
	#ifdef _WINDOWS_
	PyModule_AddIntConstant(m, "SCARD_PROTOCOL_UNDEFINED", SCARD_PROTOCOL_UNDEFINED);
	#endif
	PyModule_AddIntConstant(m, "SCARD_ABSENT", SCARD_ABSENT);
	PyModule_AddIntConstant(m, "SCARD_PRESENT", SCARD_PRESENT);
	PyModule_AddIntConstant(m, "SCARD_SWALLOWED", SCARD_SWALLOWED);
	PyModule_AddIntConstant(m, "SCARD_POWERED", SCARD_POWERED);
	PyModule_AddIntConstant(m, "SCARD_NEGOTIABLE", SCARD_NEGOTIABLE);
	PyModule_AddIntConstant(m, "SCARD_SPECIFIC", SCARD_SPECIFIC);
	#ifndef _WINDOWS_
	// PCSC-lite specific
	PyModule_AddIntConstant(m, "SCARD_PROTOCOL_ANY", SCARD_PROTOCOL_ANY);
	#endif
	PyModule_AddIntConstant(m, "SCARD_STATE_UNAWARE", SCARD_STATE_UNAWARE);
	PyModule_AddIntConstant(m, "SCARD_STATE_IGNORE", SCARD_STATE_IGNORE);
	PyModule_AddIntConstant(m, "SCARD_STATE_CHANGED", SCARD_STATE_CHANGED);
	PyModule_AddIntConstant(m, "SCARD_STATE_UNKNOWN", SCARD_STATE_UNKNOWN);
	PyModule_AddIntConstant(m, "SCARD_STATE_UNAVAILABLE", SCARD_STATE_UNAVAILABLE);
	PyModule_AddIntConstant(m, "SCARD_STATE_EMPTY", SCARD_STATE_EMPTY);
	PyModule_AddIntConstant(m, "SCARD_STATE_PRESENT", SCARD_STATE_PRESENT);
	PyModule_AddIntConstant(m, "SCARD_STATE_ATRMATCH", SCARD_STATE_ATRMATCH);
	PyModule_AddIntConstant(m, "SCARD_STATE_EXCLUSIVE", SCARD_STATE_EXCLUSIVE);
	PyModule_AddIntConstant(m, "SCARD_STATE_INUSE", SCARD_STATE_INUSE);
	PyModule_AddIntConstant(m, "SCARD_STATE_MUTE", SCARD_STATE_MUTE);

#ifdef WINDOWS
	// These values are taken from the Win32 SCard documentation
	PyModule_AddIntConstant(m, "SCARD_ATTR_SUPRESS_T1_IFS_REQUEST", SCARD_ATTR_SUPRESS_T1_IFS_REQUEST); 
	PyModule_AddIntConstant(m, "SCARD_ATTR_ATR_STRING", SCARD_ATTR_ATR_STRING);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CHANNEL_ID", SCARD_ATTR_CHANNEL_ID);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CHARACTERISTICS", SCARD_ATTR_CHARACTERISTICS);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_BWT", SCARD_ATTR_CURRENT_BWT);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_CLK", SCARD_ATTR_CURRENT_CLK);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_CWT", SCARD_ATTR_CURRENT_CWT);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_D", SCARD_ATTR_CURRENT_D);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_EBC_ENCODING", SCARD_ATTR_CURRENT_EBC_ENCODING);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_F", SCARD_ATTR_CURRENT_F);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_IFSC", SCARD_ATTR_CURRENT_IFSC);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_IFSD", SCARD_ATTR_CURRENT_IFSD);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_N", SCARD_ATTR_CURRENT_N);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_PROTOCOL_TYPE", SCARD_ATTR_CURRENT_PROTOCOL_TYPE);
	PyModule_AddIntConstant(m, "SCARD_ATTR_CURRENT_W", SCARD_ATTR_CURRENT_W);
	PyModule_AddIntConstant(m, "SCARD_ATTR_DEFAULT_CLK", SCARD_ATTR_DEFAULT_CLK);
	PyModule_AddIntConstant(m, "SCARD_ATTR_DEFAULT_DATA_RATE", SCARD_ATTR_DEFAULT_DATA_RATE);
	PyModule_AddIntConstant(m, "SCARD_ATTR_DEVICE_FRIENDLY_NAME", SCARD_ATTR_DEVICE_FRIENDLY_NAME);
	PyModule_AddIntConstant(m, "SCARD_ATTR_DEVICE_IN_USE", SCARD_ATTR_DEVICE_IN_USE);
	PyModule_AddIntConstant(m, "SCARD_ATTR_DEVICE_SYSTEM_NAME", SCARD_ATTR_DEVICE_SYSTEM_NAME);
	PyModule_AddIntConstant(m, "SCARD_ATTR_DEVICE_UNIT", SCARD_ATTR_DEVICE_UNIT);
	PyModule_AddIntConstant(m, "SCARD_ATTR_ICC_INTERFACE_STATUS", SCARD_ATTR_ICC_INTERFACE_STATUS);
	PyModule_AddIntConstant(m, "SCARD_ATTR_ICC_PRESENCE", SCARD_ATTR_ICC_PRESENCE);
	PyModule_AddIntConstant(m, "SCARD_ATTR_ICC_TYPE_PER_ATR", SCARD_ATTR_ICC_TYPE_PER_ATR);
	PyModule_AddIntConstant(m, "SCARD_ATTR_MAX_CLK", SCARD_ATTR_MAX_CLK);
	PyModule_AddIntConstant(m, "SCARD_ATTR_MAX_DATA_RATE", SCARD_ATTR_MAX_DATA_RATE);
	PyModule_AddIntConstant(m, "SCARD_ATTR_MAX_IFSD", SCARD_ATTR_MAX_IFSD);
	PyModule_AddIntConstant(m, "SCARD_ATTR_POWER_MGMT_SUPPORT", SCARD_ATTR_POWER_MGMT_SUPPORT);
	PyModule_AddIntConstant(m, "SCARD_ATTR_PROTOCOL_TYPES", SCARD_ATTR_PROTOCOL_TYPES);
	PyModule_AddIntConstant(m, "SCARD_ATTR_VENDOR_IFD_SERIAL_NO", SCARD_ATTR_VENDOR_IFD_SERIAL_NO);
	PyModule_AddIntConstant(m, "SCARD_ATTR_VENDOR_IFD_TYPE", SCARD_ATTR_VENDOR_IFD_TYPE);
	PyModule_AddIntConstant(m, "SCARD_ATTR_VENDOR_IFD_VERSION", SCARD_ATTR_VENDOR_IFD_VERSION);
	PyModule_AddIntConstant(m, "SCARD_ATTR_VENDOR_NAME", SCARD_ATTR_VENDOR_NAME);
#endif

	PyModule_AddStringConstant(m, "PYCSC_PLATFORM_PCSCWINDOWS", PYCSC_PLATFORM_PCSCWINDOWS);
	PyModule_AddStringConstant(m, "PYCSC_PLATFORM_PCSCLITE", PYCSC_PLATFORM_PCSCLITE);
#ifdef WINDOWS
	PyModule_AddStringConstant(m, "platform", PYCSC_PLATFORM_PCSCWINDOWS);
#else	
	PyModule_AddStringConstant(m, "platform", PYCSC_PLATFORM_PCSCLITE);
#endif

	//+ Define the PycscException
	PycscException = PyErr_NewException("pycsc.PycscException", NULL, NULL);
	PyModule_AddObject(m, "PycscException", PycscException);
}

/* Internal tool */
static LONG getReaderList(SCARDCONTEXT hContext, LPSTR* pmszReaders, DWORD *pdwReaders)
{
  LPCSTR mszGroups = 0;
  LPSTR mszReaders = NULL;
  ULONG  dwReaders;
  LONG rv;

  /* Read size of reader list */
  rv = SCardListReaders( hContext, mszGroups, 0, &dwReaders );

  if ( rv != SCARD_S_SUCCESS )
  {
    return rv;
  }
#ifdef DEBUG
  printf("SCardListReaders dry run successful\n");
#endif

  /* malloc a buffer to store reader names */
  mszReaders = (char *)PyMem_Malloc(sizeof(char)*dwReaders);

  if (!mszReaders)
  {
    return SCARD_E_NO_MEMORY;
  }

  /* Get the list of readers*/
  rv = SCardListReaders(hContext, mszGroups,
                        mszReaders, &dwReaders );

  if ( rv != SCARD_S_SUCCESS )
  {
    PyMem_Free((void *)mszReaders);
    return rv;
  }
#ifdef DEBUG
  printf("SCardListReaders successful\n");
#endif
  *pmszReaders = mszReaders;
  *pdwReaders  = dwReaders;
  return SCARD_S_SUCCESS;
}
