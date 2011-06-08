#include <timer.h>

#ifdef _T6IF

/********************************************************************
*    Function Name:  WriteTimer6                                    *
*    Description:    This routine writes a 16-bit value to Timer6   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer6(unsigned int timer)
{
    TMR6 = timer;     /* assign timer value to Timer6 Register */
}

#else
#warning "Does not build on this target"
#endif
