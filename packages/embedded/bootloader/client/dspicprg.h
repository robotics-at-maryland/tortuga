/* 
 * dspicprg.h -- header file for dspicprg utility
 *
 * homer reid -- 1/2005
 */

#ifndef DSPICPRG_H
#define DSPICPRG_H

#include <stdio.h>
#include <stdarg.h>

/***************************************************/
/* Miscellaneous constants *************************/
/***************************************************/

#define MCLR  0
#define DATA  1
#define CLOCK 2

/*
 * stdp programming sequences
 * (values for WhichSeq parameter to STDPSeq()
 */
#define STDP_BULKERASE  0 
#define STDP_READAPPID  1 
#define STDP_READCFG    2 
#define STDP_WRITECFG   3 
#define STDP_READDATA   4
#define STDP_WRITEDATA  5 
#define STDP_READCODE   6
#define STDP_WRITECODE  7 

/*
 * values for the Type field of the MemoryRow 
 * structure
 */
#define CODE_MEM 0
#define DATA_MEM 1
#define CNFG_MEM 2

#define DATA_MEM_MAX 0x800000
#define CNFG_MEM_MIN 0xF80000
#define CNFG_MEM_MAX 0xF800E0


/***************************************************/
/* Convenient shorthands for data types. ***********/
/***************************************************/
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

/***************************************************/
/* Data structure definitions **********************/
/***************************************************/

/* structure that encapsulates information about */
/* the different dsPIC models we support         */
struct ChipType
 { char *Name;                      /* device name */
   unsigned short ProgramWords;     /* # of 24-bit instructions */
   unsigned short DataWords;        /* # of 16-bit eeprom slots */
 };

/* each memory row structure represents a block   */
/* of memory (32 3-byte instructions for code mem,*/
/* 16 2-byte words for data mem, 14 2-byte words  */
/* for config mem) to be programmed into the chip.*/
/* The type of memory (code, data, or config) can */
/* be read off from the address:                  */
/* 0x000000 <= Address < 0x7F0000: code memory    */
/* 0x7F0000 <= Address < 0x800000: data memory    */
/* 0xF80000 <= Address < 0xF80010: cnfg memory    */
struct MemoryRow 
 { DWORD Address;
   int Type;
   BYTE *Buffer;
   struct MemoryRow *Next;
 };

/***************************************************/
/* Global variables (instantiated in dsppglob.c) ***/
/***************************************************/
extern struct ChipType SupportedChips[], *WhichChip;
struct MemoryRow *MRList;
extern int PortBase;
extern FILE *HexFile;

/***************************************************/
/* Function prototypes *****************************/                
/***************************************************/

/* in dspphex.c */
void WriteELARecord(DWORD AdrsHigh);
void WriteDataRecord(WORD AdrsLow, BYTE *Buffer);
void WriteEOFRecord();
void ReadHexFile();

/* in dspphard.c */
void HWInit();
void NanoDelay(long NS);
void Set(int Which, int Value);
int GetData();

/* in dsppmisc.c */
void ErrExit(char *Format, ... );
void *SafeMalloc(int Size);

/* in dsppmrow.c */
struct MemoryRow *GetMemoryRow(DWORD Address);

/* in dspppack.c */
void CodePack(BYTE *UBuf, BYTE *PBuf);
void CodeUnpack(BYTE *PBuf, BYTE *UBuf);
void DataPack(BYTE *UBuf, BYTE *PBuf);
void DataUnpack(BYTE *PBuf, BYTE *UBuf);
void CnfgPack(BYTE *UBuf, BYTE *PBuf);

/* in dsppstdp.c */
void EnterSTDPMode();
void ExitSTDPMode();
void STDPSeq(int WhichSeq, DWORD DestAddr, BYTE *InBuf, BYTE *OutBuf);

#endif /* #ifndef DSPICPRG_H */
