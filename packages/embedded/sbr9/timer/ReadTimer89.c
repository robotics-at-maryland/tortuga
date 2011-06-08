#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  ReadTimer89                                   *
*    Description:    This routine reads the 32-bit value from       *
*                    Timer9 and Timer8.                             *
*    Parameters:     None                                           *
*    Return Value:   unsigned long: Timer8 and Timer9 32-bit value *
********************************************************************/

unsigned long ReadTimer89(void)
{
    unsigned long timer;
    timer = TMR8;                        /* Copy Timer8 into timer low 16bit */
    timer |= (unsigned long)TMR9HLD<<16; /* shift  16 time as TMR 9
                                            contains MSB , TMR8 LSB */ 
    return (timer);
}

#else
#warning "Does not build on this target"
#endif
