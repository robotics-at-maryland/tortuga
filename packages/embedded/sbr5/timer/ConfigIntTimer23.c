#include <timer.h>

#ifdef _T3IF

/*******************************************************************
*    Function Name: ConfigIntTimer23                               *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Parameters:    unsigned int config                            *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer23(unsigned int config)
{
    _T3IF = 0;                   /* clear IF bit */
    _T3IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T3IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
