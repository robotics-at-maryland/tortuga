#include <timer.h>

#ifdef _T4IF

/********************************************************************
*    Function Name:  ReadTimer4                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer4 Register.                               *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer4(void)
{
    return (TMR4);      /* Return the Timer4 register */
}

#else
#warning "Does not build on this target"
#endif
