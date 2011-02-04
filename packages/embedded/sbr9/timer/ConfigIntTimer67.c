#include <timer.h>

#ifdef _T7IF

/*******************************************************************
*    Function Name: ConfigIntTimer67                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer67(unsigned int config)
{
    _T7IF = 0;                   /* clear IF bit */
    _T7IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T7IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
