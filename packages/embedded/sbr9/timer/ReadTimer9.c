#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  ReadTimer9                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer Register.                                *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer9(void)
{
    return (TMR9);      /* Return the Timer9 register */
}

#else
#warning "Does not build on this target"
#endif
