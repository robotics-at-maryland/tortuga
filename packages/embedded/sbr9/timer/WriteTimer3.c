#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  WriteTimer3                                    *
*    Description:    This routine writes a 16-bit value to Timer3   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer3(unsigned int timer)
{
    TMR3 = timer;     /* assign timer value to Timer3 Register */
}

#else
#warning "Does not build on this target"
#endif
