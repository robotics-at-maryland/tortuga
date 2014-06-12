#include <outcompare.h>

#ifdef _OC8IF

/************************************************************************
*    Function Name:  ConfigIntOC8                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC8(unsigned int config)
{
    _OC8IF = 0;                   /* Clear IF bit */
    _OC8IP = (config &0x0007);    /* Assign Interrupt Priority */
    _OC8IE = (config &0x0008)>>3; /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
