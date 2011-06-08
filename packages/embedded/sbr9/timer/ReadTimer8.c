#include <timer.h>

#ifdef _T8IF

/********************************************************************
*    Function Name:  ReadTimer8                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer Register.                                *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer8(void)
{
    return (TMR8);      /* Return the Timer8 register */
}

#else
#warning "Does not build on this target"
#endif
