#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  WriteTimer7                                    *
*    Description:    This routine writes a 16-bit value to Timer7   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer7(unsigned int timer)
{
    TMR7 = timer;     /* assign timer value to Timer7 Register */
}

#else
#warning "Does not build on this target"
#endif
