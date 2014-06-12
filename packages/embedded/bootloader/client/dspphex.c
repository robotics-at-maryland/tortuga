/* 
 * dspphex.c -- routines for reading and writing intel hex files
 *           -- for dspicprg and dspicdmp utilities
 *
 * homer reid -- 1/2005
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dspicprg.h"

#ifdef DEBUG
void PrintMemRows();
#endif /* DEBUG */

/***************************************************/
/* Write an extended linear address record to the **/
/* hexfile.                                       **/
/* The AdrsHigh parameter is the upper word of    **/
/* the address in the PIC, which means we have to **/
/* double it before writing to the hex file since **/
/* addresses in the hex file are 2x addresses in  **/
/* the PIC.                                       **/
/***************************************************/
void WriteELARecord(DWORD Address)
{ 
 BYTE CheckSum;

 /* get the upper word of the doubled address */
 Address=(Address>>15) & 0xFFFF;
 CheckSum=1 + ~(0x02 + 0x04 + (Address&0xFF) + ((Address&0xFF00)>>8) );
 fprintf(HexFile,":02000004%04X%02X\n",Address,CheckSum);

} 

/***************************************************/
/* Write a 16-bit data record to the hexfile.      */
/* As above, double the address before writing to  */
/* the hexfile.                                    */
/***************************************************/
void WriteDataRecord(WORD AdrsLow, BYTE *Buffer)
{ BYTE CheckSum;
  char OutString[33];
  int i;

  AdrsLow*=2;
  CheckSum=0x10 + (AdrsLow&0xFF) + ((AdrsLow&0xFF00)>>8);
  for (i=0; i<16; i++) 
   { CheckSum+=Buffer[i];
     sprintf(OutString+i*2,"%02X",Buffer[i]);
   };
  CheckSum=1 + ~CheckSum;
  
  fprintf(HexFile,":10%04X00%s%02X\n",AdrsLow,OutString,CheckSum);
}

/***************************************************/
/* Write an end-of-file record to the hexfile.     */
/***************************************************/
void WriteEOFRecord()
{ fprintf(HexFile,":00000001FF\n");
}

/***************************************************/
/* Read through the hex file one line at a time    */
/* and construct a list of 'MemoryRows' which we   */
/* will later write into the chip.                 */
/* We do this because code memory can only be      */
/* programmed in 32-byte chunks (and data in       */
/* 16-byte chunks) at a time, and different        */
/* portions of one chunk might be specified in     */
/* different places in the hex file.               */
/* So rather than reading through the hex file and */
/* programming each line's worth of information    */
/* into the chip as we read it, it makes sense to  */
/* aggregate all information from the entire file  */
/* and then burn the chip all at once.             */
/***************************************************/
void ReadHexFile()
{ 
  WORD AddressHigh, AddressLow;
  unsigned long IdentWord;
  char FileBuffer[100], Buf2[3], c;
  int i, j, LineNo, RecLen, RecType, Offset;
  struct MemoryRow *MR;

  AddressHigh=0;
  LineNo=0;
  Buf2[2]=0;
  while ( fgets(FileBuffer,100,HexFile) )
   { 
     LineNo++;

     if ( FileBuffer[0]!=':' ) 
      ErrExit("invalid hex file (line %i)",LineNo);

     /* save 9th character, then zero out that */
     /* slot so we can use strtoul to decode   */
     /* the record length, address, and type   */
     c=FileBuffer[9];
     FileBuffer[9]=0;
     IdentWord=strtoul(FileBuffer+1, 0, 16);
     FileBuffer[9]=c;  /* replace 9th character */
     RecLen=(IdentWord >> 24 ) & 0xFF;
     AddressLow=(IdentWord >> 8 ) & 0x00FFFF;
     RecType=IdentWord & 0xFF;

     switch(RecType)
      { 
        /* end-of-file record */
        case 0x01:
         fclose(HexFile);
	   #ifdef DEBUG
	     PrintMemRows();
	   #endif
         return; 

        /* extended linear address record */
        case 0x04:
         if ( RecLen!=0x02 || AddressLow!=0 )
           ErrExit("invalid hex file (line %i)",LineNo);
         FileBuffer[13]=0;
         AddressHigh=strtoul(FileBuffer+9,0,16);
         break;

        /* data record */
        case 0x00:

         /*
          * data records that refer to configuration memory
          * are handled differently from those that refer to 
          * code and data memory.
          */
         if (AddressHigh==0x00F8)     /* config memory */
          { 
            /* create a separate MemoryRow structure     */
            /* for each two-byte word in the data record */
            for (i=0, j=9; i<RecLen; i+=4)
             { MR=GetMemoryRow( AddressHigh<<16 | AddressLow );
               Buf2[0]=FileBuffer[j++];
               Buf2[1]=FileBuffer[j++];
               MR->Buffer[0]=(BYTE)strtoul(Buf2,0,16);
               Buf2[0]=FileBuffer[j++];
               Buf2[1]=FileBuffer[j++];
               MR->Buffer[1]=(BYTE)strtoul(Buf2,0,16);
               j+=4; /* skip padding bytes */
             }; /* for (i=0, j=9; i<RecLen; i+=4) */
          }
         else /* code and data memory */
          {
             /* find or create a MemoryRow structure  */
             /* for the row containing this address   */
            if ( !(MR=GetMemoryRow( (AddressHigh<<16) | AddressLow ) ) )
             ErrExit("hex file line %i refers to unimplemented memory",LineNo); 
    
             /* convert AddressLow into an offset into the row */
             /* by subtracting the starting address of the row */
            AddressLow-=2*((MR->Address)&0xFFFF); 

            /* read contents of data record into the     */
            /* memory buffer in the MemoryRow structure. */
            for (Offset=0,i=0, j=9; i<RecLen; i++)
             { Buf2[0]=FileBuffer[j++];
               Buf2[1]=FileBuffer[j++];

	       if ((AddressLow+i+Offset) >= 128) //We gotta move on to next Row
		 {
		   MR = GetMemoryRow((MR->Address + 0x40)<<1);
		   AddressLow = 0;
		   Offset = i * -1;
		 }

               MR->Buffer[AddressLow+i+Offset]=(BYTE)strtoul(Buf2,0,16);
             };
          }; /* if (AddressHigh==0x00F8) */
         break; /* case '0x00:' */

        /* unknown record */
        default: 
         ErrExit("unknown record type on line %i of hex file",LineNo); 

      }; /* switch(RecType) */

   }; /* while fgets... */

}

