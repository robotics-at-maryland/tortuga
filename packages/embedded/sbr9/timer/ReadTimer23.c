#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  ReadTimer23                                    *
*    Description:    This routine reads the 32-bit value from       *
*                    Timer3 and Timer2.                             *
*    Parameters:     None                                           *
*    Return Value:   unsigned long: Timer2 and Timer3  32-bit value *
********************************************************************/

unsigned long ReadTimer23(void)
{
    unsigned long timer;
    timer = TMR2;                        /* Copy Timer2 into timer low 16bit */
    timer |= (unsigned long)TMR3HLD<<16; /* shift  16 time as TMR 3 
                                            contains MSB , TMR2 LSB */ 
    return (timer);
}

#else
#warning "Does not build on this target"
#endif
