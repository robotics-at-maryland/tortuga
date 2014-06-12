#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  WriteTimer23                                   *
*    Description:    This routine writes a LS word                  *
*                    to TMR2 and MS word into TMR3                  *
*    Return Value:   None                                           *
*    Parameters:     unsigned int: value to write to TMR2 and TMR3  *
********************************************************************/

void WriteTimer23( unsigned long timer)
{
    TMR3HLD = (timer>>16);
    TMR2 = timer;       /* LSB to Timer2 and MSB to Timer3HLD */    
}

#else
#warning "Does not build on this target"
#endif
