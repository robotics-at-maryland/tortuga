#include <outcompare.h>

#ifdef _OC5IF

/************************************************************************
*    Function Name:  ConfigIntOC5                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC5(unsigned int config)
{
    _OC5IF = 0;                   /* Clear IF bit */
    _OC5IP = (config &0x0007);    /* Assign Interrupt Priority */
    _OC5IE = (config &0x0008)>>3; /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
