#include <timer.h>

#ifdef _T1IF

/*******************************************************************
*    Function Name: ConfigIntTimer1                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer1(unsigned int config)
{
    _T1IF = 0;                   /* clear IF bit */
    _T1IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T1IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
