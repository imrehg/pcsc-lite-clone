/*****************************************************************
/
/ File   :   pcscdefines.h
/ Author :   David Corcoran <corcoran@linuxnet.com>
/ Date   :   June 15, 2000
/ Purpose:   This provides PC/SC shared defines.
/            See http://www.linuxnet.com for more information.
/ License:   See file LICENSE
/
******************************************************************/

#ifndef _pcscdefines_h_
#define _pcscdefines_h_

#ifdef __cplusplus
extern "C" {
#endif 

/* Defines a list of pseudo types. */

  typedef unsigned long      DWORD;
  typedef unsigned long*     PDWORD;
#ifndef __COREFOUNDATION_CFPLUGIN__
  typedef unsigned long      ULONG;
#endif
  typedef unsigned long*     PULONG;
  typedef unsigned char      UCHAR;
  typedef unsigned char*     PUCHAR;
  typedef char*              LPSTR;
  typedef long               RESPONSECODE;
  typedef void               VOID;

  #define STATUS_SUCCESS               0xFA
  #define STATUS_UNSUCCESSFUL          0xFB
  #define STATUS_COMM_ERROR            0xFC
  #define STATUS_DEVICE_PROTOCOL_ERROR 0xFD

  #define MAX_RESPONSE_SIZE  264
  #define MAX_BUFFER_SIZE    264
  #define MAX_ATR_SIZE       33

#ifdef __cplusplus
}
#endif

#endif /* _pcscdefines_h_ */
