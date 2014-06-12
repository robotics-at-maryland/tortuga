#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  WriteTimer67                                   *
*    Description:    This routine writes a LS word                  *
*                    to TMR6 and MS word into TMR7                 *
*    Return Value:   None                                           *
*    Parameters:     unsigned int: value to write to TMR6 and TMR7  *
********************************************************************/

void WriteTimer67( unsigned long timer)
{
    TMR7HLD = (timer>>16);
    TMR6 = timer;       /* LSB to Timer6 and MSB to Timer7HLD */    
}

#else
#warning "Does not build on this target"
#endif
