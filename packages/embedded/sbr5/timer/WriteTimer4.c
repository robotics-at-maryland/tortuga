#include <timer.h>

#ifdef _T4IF

/********************************************************************
*    Function Name:  WriteTimer4                                    *
*    Description:    This routine writes a 16-bit value to Timer4   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer4(unsigned int timer)
{
    TMR4 = timer;     /* assign timer value to Timer4 Register */
}

#else
#warning "Does not build on this target"
#endif
