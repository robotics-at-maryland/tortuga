#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  WriteTimer45                                   *
*    Description:    This routine writes a least 16-bit value       *
*                    to Timer4 and Msb into Timer5                  *
*    Parameters:     unsigned long				    *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer45( unsigned long timer)
{   
    TMR5HLD = (timer>>16);
    TMR4 = timer;       /* LSB to Timer4 and MSB to Timer5HLD */          
}

#else
#warning "Does not build on this target"
#endif
