#include <timer.h>

#ifdef _T1IF

/********************************************************************
*    Function Name:  WriteTimer1                                    *
*    Description:    This routine writes a 16-bit value to Timer1   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer1(unsigned int timer)
{
    TMR1 = timer;     /* assign timer value to Timer1 Register */
}

#else
#warning "Does not build on this target"
#endif
