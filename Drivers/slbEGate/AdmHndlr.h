/*****************************************************************
/
/ File   :   AdmHndlr.h
/ Author :   David Corcoran
/ Date   :   October 15, 1999
/ Purpose:   This handles administrative functions like reset/power.
/            See http://www.linuxnet.com for more information.
/ License:   See file LICENSE
/
******************************************************************/

#ifndef __AdmHndlr_h__
#define __AdmHndlr_h__

#define ADM_PORT_COM1    0x0001
#define ADM_PORT_COM2    0x0002
#define ADM_PORT_COM3    0x0004
#define ADM_PORT_COM4    0x0008
#define ADM_PORT_USB     0x0010

#define ADM_CARDTYPE_T0  0x0C
#define ADM_CARDTYPE_T1  0x0D

#define ADM_NOTIFY_TRUE  0x01
#define ADM_NOTIFY_FALSE 0x02

#define ADM_MAX_TIMEOUT  10


//DWORD Adm_ResetICC( DWORD, PUCHAR, PDWORD );
DWORD Adm_TransmitICC( DWORD, PUCHAR, DWORD, PUCHAR, PDWORD );

#endif
