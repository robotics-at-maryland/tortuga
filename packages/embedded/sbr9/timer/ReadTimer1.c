#include <timer.h>

#ifdef _T1IF

/********************************************************************
*    Function Name:  ReadTimer1                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer Register.                                *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer1(void)
{
    return (TMR1);      /* Return the Timer1 register */
}

#else
#warning "Does not build on this target"
#endif
