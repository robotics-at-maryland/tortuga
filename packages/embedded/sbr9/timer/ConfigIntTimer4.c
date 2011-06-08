#include <timer.h>

#ifdef _T4IF

/*******************************************************************
*    Function Name: ConfigIntTimer4                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Parameters:    unsigned int config                            *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer4(unsigned int config)
{
    _T4IF = 0;                   /* clear IF bit */
    _T4IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T4IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
