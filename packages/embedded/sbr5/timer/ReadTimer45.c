#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  ReadTimer45                                    *
*    Description:    This routine reads the 32-bit value from       *
*                    Timer4 and Timer5.                             *
*    Parameters:     None                                           *
*    Return Value:   unsigned long: Timer4 and Timer5  32-bit value *
********************************************************************/

unsigned long ReadTimer45(void)
{
    unsigned long timer;
    timer = TMR4;                        /* Copy Timer4 into timer low 16bit */
    timer |= (unsigned long)TMR5HLD<<16; /* shift  16 time as TMR 5 
                                            contains MSB , TMR4 LSB */ 
    return (timer);
}

#else
#warning "Does not build on this target"
#endif
