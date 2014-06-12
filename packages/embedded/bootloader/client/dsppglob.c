/* 
 * dsppglob.c -- global variables for dspicprg utility
 *
 * homer reid -- 1/2005
 */

#include <stdio.h>
#include "dspicprg.h"

/*
 * ChipType structures defining the various 
 * models of dsPIC we support
 */
struct ChipType SupportedChips[]=
  { { "pic30f2010", 0x1000, 0x200 },
    { "pic30f3010", 0x2000, 0x200 },
    { "pic30f4012", 0x4000, 0x200 },
    { "pic30f3011", 0x2000, 0x200 },
    { "pic30f4011", 0x4000, 0x200 },
    { "pic30f5015", 0x5800, 0x200 },
    { "pic30f6010", 0xC000, 0x800 },
    { 0,0,0}
  };

/* 
 * General global variables, initialized to 
 * their default values.
 */
int PortBase=0x3F8;
struct ChipType *WhichChip=SupportedChips;
struct MemoryRow *MRList=0;
FILE *HexFile=0;
