#include <timer.h>

#ifdef _T2IF

/*******************************************************************
*    Function Name: ConfigIntTimer2                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Parameters:    unsigned int config                            *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer2(unsigned int config)
{
    _T2IF = 0;                   /* clear IF bit */
    _T2IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T2IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
