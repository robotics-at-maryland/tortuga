#include <timer.h>

#ifdef _T6IF

/********************************************************************
*    Function Name:  ReadTimer6                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer Register.                                *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer6(void)
{
    return (TMR6);      /* Return the Timer6 register */
}

#else
#warning "Does not build on this target"
#endif
