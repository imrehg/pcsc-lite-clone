/*
 * @License IBM
 */

/*
 * Author:  Stephan Breideneich (sbreiden@de.ibm.com)
 * Version: $Id$
 */

#include <jni.h>

#define PCSC_EXCEPTION_CLASS "com/ibm/opencard/terminal/pcsc10/PcscException"


/*
 * throwPcscException
 *
 * throws an PcscException with the return code informations of the PC/SC
 *
 * return: 0 for success, other values for exception occured
 */
int throwPcscException(JNIEnv *env, jobject obj, const char *method, const char *msg, long returnCode);

// $Log$
// Revision 1.1  2001/11/21 01:00:11  corcoran
// Initial revision
//
// Revision 1.2  1998/04/20 13:10:54  breid
// PackagePath corrected
//
