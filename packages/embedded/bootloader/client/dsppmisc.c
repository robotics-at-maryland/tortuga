/* 
 * dsppmisc.c -- miscellaneous routines for dspicprg utility
 *
 * homer reid -- 1/2005
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "dspicprg.h"

/***************************************************/
/* ErrExit: print an error message and quit. *******/
/***************************************************/
void ErrExit(char *format, ...)
{
 va_list ap;
 char buffer[200];

 va_start(ap,format);
 vsnprintf(buffer,200,format,ap);
 fprintf(stderr,"error: %s (aborting)\n",buffer);
 va_end(ap);
 exit(1);

}

/***************************************************/
/* SafeMalloc: malloc() with error checking ********/
/***************************************************/
void *SafeMalloc(int size)
{ 
  void *p=calloc(1, size);
 if (!p) ErrExit("out of memory");
 return p;
}
