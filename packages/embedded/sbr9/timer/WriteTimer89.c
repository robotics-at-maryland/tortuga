#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  WriteTimer89                                   *
*    Description:    This routine writes a LS word                  *
*                    to TMR8 and MS word into TMR9                *
*    Return Value:   None                                           *
*    Parameters:     unsigned int: value to write to TMR8 and TMR9  *
********************************************************************/

void WriteTimer89( unsigned long timer)
{
    TMR9HLD = (timer>>16);
    TMR8 = timer;       /* LSB to Timer8 and MSB to Timer9HLD */         
}

#else
#warning "Does not build on this target"
#endif
