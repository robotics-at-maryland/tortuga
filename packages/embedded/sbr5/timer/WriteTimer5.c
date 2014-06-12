#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  WriteTimer5                                    *
*    Description:    This routine writes a 16-bit value to Timer5   *
*    Parameters:     int: value to write to Timer                   *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer5(unsigned int timer)
{
    TMR5 = timer;     /* assign timer value to Timer5 Register */
}

#else
#warning "Does not build on this target"
#endif
