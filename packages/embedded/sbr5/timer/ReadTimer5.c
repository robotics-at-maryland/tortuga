#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  ReadTimer5                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer5 Register.                               *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer5(void)
{
    return (TMR5);      /* Return the Timer5 register */
}

#else
#warning "Does not build on this target"
#endif
