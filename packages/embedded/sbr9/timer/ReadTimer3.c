#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  ReadTimer3                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer3 Register.                               *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer3(void)
{
    return (TMR3);      /* Return the Timer3 register */
}

#else
#warning "Does not build on this target"
#endif
