#include <outcompare.h>

#ifdef _OC2IF

/************************************************************************
*    Function Name:  ConfigIntOC2                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC2(unsigned int config)
{
    _OC2IF = 0;                   /* Clear IF bit */
    _OC2IP = (config &0x0007);    /* Assign Interrupt Priority */ 
    _OC2IE = (config &0x0008)>>3; /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
