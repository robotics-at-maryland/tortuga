#include <timer.h>

#ifdef _T2IF

/********************************************************************
*    Function Name:  ReadTimer2                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer2 Register.                               *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer2(void)
{
    return (TMR2);      /* Return the Timer2 register */
}

#else
#warning "Does not build on this target"
#endif
