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
************************************************************/

#include <Python.h>

/* Include the header files */

#ifdef __APPLE__
#include <wintypes.h>
#include <PCSC/winscard.h>
#else
#include <winscard.h>
#endif

/* Internal tool */
static LONG getReaderList(SCARDCONTEXT hContext, LPSTR* pmszReaders, 
                          DWORD *pdwReaders);

#ifdef _WINDOWS_
/* Windows does not provide the pcsc_stringify_error function */
char ErrorString[200];
char *pcsc_stringify_error(LONG rv)
{
	char strErrorCode[50];
	switch(rv) 
	{
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
		case SCARD_E_NOT_READY:
			strcpy(strErrorCode, "SCARD_E_NOT_READY");
			break;
		case SCARD_E_NOT_TRANSACTED:
			strcpy(strErrorCode, "SCARD_E_NOT_TRANSACTED");
			break;
		case SCARD_E_NO_MEMORY:
			strcpy(strErrorCode, "SCARD_E_NO_MEMORY");
			break;
		case SCARD_E_NO_SERVICE:
			strcpy(strErrorCode, "SCARD_E_NO_SERVICE");
			break;
		case SCARD_E_NO_SMARTCARD:
			strcpy(strErrorCode, "SCARD_E_NO_SMARTCARD");
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
    sprintf(ErrorString, "Error returned by PCSC: 0x%08X (%s)", rv, strErrorCode);
    return ErrorString;
}
#endif

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
static PyObject * pycsc_reconnect( PyObject *self,  PyObject *args,  PyObject *keywds)
{
  pycscobject *object = (pycscobject *)self;
  LONG rv;
  DWORD dwPreferredProtocol = SCARD_PROTOCOL_T0;
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
#endif
  rv = SCardReconnect(object->hCard, object->sMode, dwPreferredProtocol,
              dwInitialization, &pdwActiveProtocol);

  if ( rv != SCARD_S_SUCCESS )
    {
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
      return NULL;
    }

  Py_INCREF(Py_None);
  return Py_None;
}

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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
      return NULL;
    }
#ifdef DEBUG
  printf("pycsc_disconnect : OK\n");
#endif

  Py_INCREF(Py_None);
  return Py_None;
}

/* Returns a dictionary with keys as follows:
                            "ReaderName"
                            "State"
                            "Protocol"
                            "ATR"
*/
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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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

static PyObject * pycsc_control(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}


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

/* args : a string representing data to send */
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
    PyErr_SetString(PycscException, pcsc_stringify_error(rv));
    return NULL;
  }

  return Py_BuildValue("s#", resparray, resplength);
}

static PyObject * pycsc_cancel(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}

static PyObject * pycsc_beginTransaction(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}

static PyObject * pycsc_endTransaction(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}

static PyObject * pycsc_cancelTransaction(PyObject *self, PyObject * args)
{
  PyErr_SetString(PycscException, "Not implemented");
  return NULL;
}


/* Declaration of methods */
static struct PyMethodDef pycsc_methods[] =
{
 {"reconnect",(PyCFunction)pycsc_reconnect,     METH_VARARGS|METH_KEYWORDS},
 {"disconnect",        pycsc_disconnect,        METH_VARARGS},
 {"status",            pycsc_status,            METH_VARARGS},
 {"control",           pycsc_control,           METH_VARARGS},
 {"transmit",(PyCFunction)pycsc_transmit,       METH_VARARGS|METH_KEYWORDS},
 {"cancel",            pycsc_cancel,            METH_VARARGS},
 {"beginTransaction",  pycsc_beginTransaction,  METH_VARARGS},
 {"endTransaction",    pycsc_endTransaction,    METH_VARARGS},
 {"cancelTransaction", pycsc_cancelTransaction, METH_VARARGS},
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
static PyObject * pycscobject_pycsc(PyObject *self, PyObject * args, PyObject *keywds)
{
  /* No reader name in args, connect to the first reader */
  LPSTR mszReaders = NULL;
  LPSTR szRequestedReader = "";
  DWORD dwReaders;
  DWORD dwMode = SCARD_SHARE_SHARED;
  DWORD eProtocol;   /* effective protocol */
  DWORD dwPreferredProtocol = SCARD_PROTOCOL_T0;
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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
    PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
    PyErr_SetString(PycscException, pcsc_stringify_error(rv));
    return NULL;
  }
  rv = getReaderList(hContext, &mszReaders,  &dwReaders);

  if ( rv != SCARD_S_SUCCESS )
  {
    PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
      PyErr_SetString(PycscException, pcsc_stringify_error(rv));
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
  PyObject *m, *d;

  /* patch object type for building dll on windows... */
  PycscType.ob_type = &PyType_Type;
  m = Py_InitModule("pycsc", pycsctype_methods);
  d = PyModule_GetDict(m);

  //+ Add error code and constants definitions
  PyDict_SetItemString(d, "SCARD_LEAVE_CARD", PyInt_FromLong(SCARD_LEAVE_CARD)); 
  PyDict_SetItemString(d, "SCARD_RESET_CARD", PyInt_FromLong(SCARD_RESET_CARD));
  PyDict_SetItemString(d, "SCARD_SHARE_SHARED", PyInt_FromLong(SCARD_SHARE_SHARED));
  PyDict_SetItemString(d, "SCARD_SHARE_EXCLUSIVE", PyInt_FromLong(SCARD_SHARE_EXCLUSIVE));
  PyDict_SetItemString(d, "SCARD_SHARE_DIRECT", PyInt_FromLong(SCARD_SHARE_DIRECT));
  
  PyDict_SetItemString(d, "SCARD_PROTOCOL_T0", PyInt_FromLong(SCARD_PROTOCOL_T0));
  PyDict_SetItemString(d, "SCARD_PROTOCOL_T1", PyInt_FromLong(SCARD_PROTOCOL_T1));
  PyDict_SetItemString(d, "SCARD_PROTOCOL_RAW", PyInt_FromLong(SCARD_PROTOCOL_RAW));
#ifdef _WINDOWS_
  PyDict_SetItemString(d, "SCARD_PROTOCOL_UNDEFINED", PyInt_FromLong(SCARD_PROTOCOL_UNDEFINED));
#endif
  PyDict_SetItemString(d, "SCARD_ABSENT", PyInt_FromLong(SCARD_ABSENT));
  PyDict_SetItemString(d, "SCARD_PRESENT", PyInt_FromLong(SCARD_PRESENT));
  PyDict_SetItemString(d, "SCARD_SWALLOWED", PyInt_FromLong(SCARD_SWALLOWED));
  PyDict_SetItemString(d, "SCARD_POWERED", PyInt_FromLong(SCARD_POWERED));
  PyDict_SetItemString(d, "SCARD_NEGOTIABLE", PyInt_FromLong(SCARD_NEGOTIABLE));
  PyDict_SetItemString(d, "SCARD_SPECIFIC", PyInt_FromLong(SCARD_SPECIFIC));
#ifndef _WINDOWS_
  // PCSC-lite specific
  PyDict_SetItemString(d, "SCARD_PROTOCOL_ANY", PyInt_FromLong(SCARD_PROTOCOL_ANY));
#endif
  PyDict_SetItemString(d, "SCARD_STATE_UNAWARE", PyInt_FromLong(SCARD_STATE_UNAWARE)); 
  PyDict_SetItemString(d, "SCARD_STATE_IGNORE", PyInt_FromLong(SCARD_STATE_IGNORE)); 
  PyDict_SetItemString(d, "SCARD_STATE_CHANGED", PyInt_FromLong(SCARD_STATE_CHANGED)); 
  PyDict_SetItemString(d, "SCARD_STATE_UNKNOWN", PyInt_FromLong(SCARD_STATE_UNKNOWN)); 
  PyDict_SetItemString(d, "SCARD_STATE_UNAVAILABLE", PyInt_FromLong(SCARD_STATE_UNAVAILABLE)); 
  PyDict_SetItemString(d, "SCARD_STATE_EMPTY", PyInt_FromLong(SCARD_STATE_EMPTY)); 
  PyDict_SetItemString(d, "SCARD_STATE_PRESENT", PyInt_FromLong(SCARD_STATE_PRESENT)); 
  PyDict_SetItemString(d, "SCARD_STATE_ATRMATCH", PyInt_FromLong(SCARD_STATE_ATRMATCH)); 
  PyDict_SetItemString(d, "SCARD_STATE_EXCLUSIVE", PyInt_FromLong(SCARD_STATE_EXCLUSIVE)); 
  PyDict_SetItemString(d, "SCARD_STATE_INUSE", PyInt_FromLong(SCARD_STATE_INUSE)); 
  PyDict_SetItemString(d, "SCARD_STATE_MUTE", PyInt_FromLong(SCARD_STATE_MUTE)); 
  //+ Define the PycscException
  PycscException = PyErr_NewException("pycsc.PycscException", NULL, NULL);
  PyDict_SetItemString(d, "PycscException", PycscException);
}

/* Internal tool */
static LONG getReaderList(SCARDCONTEXT hContext, LPSTR* pmszReaders, DWORD *pdwReaders)
{
  LPCSTR mszGroups = 0;
  LPSTR mszReaders = NULL;
  LONG  dwReaders;
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
