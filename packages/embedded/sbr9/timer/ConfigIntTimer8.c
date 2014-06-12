#include <timer.h>

#ifdef _T8IF

/*******************************************************************
*    Function Name: ConfigIntTimer8                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer8(unsigned int config)
{
    _T8IF = 0;                   /* clear IF bit */
    _T8IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T8IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
