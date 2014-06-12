#include <timer.h>

#ifdef _T8IF

/********************************************************************
*    Function Name:  WriteTimer8                                    *
*    Description:    This routine writes a 16-bit value to Timer8   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer8(unsigned int timer)
{
    TMR8 = timer;     /* assign timer value to Timer8 Register */
}

#else
#warning "Does not build on this target"
#endif
