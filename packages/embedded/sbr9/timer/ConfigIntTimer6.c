#include <timer.h>

#ifdef _T6IF

/*******************************************************************
*    Function Name: ConfigIntTimer6                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer6(unsigned int config)
{
    _T6IF = 0;                   /* clear IF bit */
    _T6IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T6IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
