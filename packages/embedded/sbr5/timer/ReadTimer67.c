#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  ReadTimer67                                   *
*    Description:    This routine reads the 32-bit value from       *
*                    Timer7 and Timer6.                             *
*    Parameters:     None                                           *
*    Return Value:   unsigned long: Timer6 and Timer7  32-bit value *
********************************************************************/

unsigned long ReadTimer67(void)
{
    unsigned long timer;
    timer = TMR6;                        /* Copy Timer6 into timer low 16bit */
    timer |= (unsigned long)TMR7HLD<<16; /* shift  16 time as TMR 7 
                                            contains MSB , TMR6 LSB */ 
    return (timer);
}

#else
#warning "Does not build on this target"
#endif
