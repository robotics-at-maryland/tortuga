#include <timer.h>

#ifdef _T2IF

/********************************************************************
*    Function Name:  WriteTimer2                                    *
*    Description:    This routine writes a 16-bit value to Timer2   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer2(unsigned int timer)
{
    TMR2 = timer;     /* assign timer value to Timer2 Register */
}

#else
#warning "Does not build on this target"
#endif
