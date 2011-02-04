#include <timer.h>

#ifdef _T9IF

/*******************************************************************
*    Function Name: ConfigIntTimer9                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer9(unsigned int config)
{
    _T9IF = 0;                   /* clear IF bit */
    _T9IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T9IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
