/* 
 * dsppmrow.c -- routines for dealing with 'MemoryRow'
 *            -- structures for dspicprg utilities
 *
 * homer reid -- 1/2005
 */
#include <stdlib.h>
#include <string.h>
#include "dspicprg.h"

/***************************************************/
/* Look through our global list of memory row      */
/* structures for the row corresponding to this    */
/* address. If we don't have one, create a new one */
/* and add it to the global list.                  */
/* Note that the Address parameter that is passed  */
/* in is the address as read from a hex file,      */
/* which means it is 2x the actual address in the  */
/* dsPIC.                                          */
/***************************************************/
struct MemoryRow *GetMemoryRow(DWORD Address)
{ 
  int Type, Length;
  struct MemoryRow *MR;

  static DWORD CodeMemMax=0, DataMemMin;

  /* initialize these variables on first invocation */
  if (CodeMemMax==0)
   { CodeMemMax=2*(WhichChip->ProgramWords);
     DataMemMin=0x800000-2*(WhichChip->DataWords);
   };

  /*
   * Compute the starting address of the row containing
   * this address, and determine the type of memory
   * to which it refers.
   */
  Address>>=1; /* see comment above */
  if ( Address < CodeMemMax )                               /* code memory */
   { /* code memory rows are aligned to 64-byte bndries */
     Address&=0xFFFFFC0;
     Type=CODE_MEM;
     Length=128;
   }
  else if ( DataMemMin <= Address && Address < DATA_MEM_MAX )   /* data memory */
   { 
     /* data memory rows are aligned to 32-byte bndries */
     Address&=0xFFFFFC0;
     Type=DATA_MEM;
     Length=64;
   }
  else if ( CNFG_MEM_MIN <= Address && Address < CNFG_MEM_MAX ) /* cnfg memory */
   { 
     Type=CNFG_MEM;
     Length=4;
   }
  else /* invalid address */
   return 0;

  /* look for existing MemoryRow structure for this row */
  for (MR=MRList; MR; MR=MR->Next)
   if (MR->Address==Address) 
    return MR;

  /* none was found; create and return pointer to new one */
  MR=SafeMalloc( sizeof(*MR) );
  MR->Address=Address;
  MR->Type=Type;
  MR->Buffer=SafeMalloc( Length );
  MR->Next=MRList;
  MRList=MR;
  return MR;

}
