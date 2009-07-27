#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  ReadTimer7                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer Register.                                *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer7(void)
{
    return (TMR7);      /* Return the Timer7 register */
}

#else
#warning "Does not build on this target"
#endif
