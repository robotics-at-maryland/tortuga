#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  WriteTimer9                                    *
*    Description:    This routine writes a 16-bit value to Timer9   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer9(unsigned int timer)
{
    TMR9 = timer;     /* assign timer value to Timer9 Register */
}

#else
#warning "Does not build on this target"
#endif
