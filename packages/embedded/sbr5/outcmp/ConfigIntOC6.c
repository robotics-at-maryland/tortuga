#include <outcompare.h>

#ifdef _OC6IF

/************************************************************************
*    Function Name:  ConfigIntOC6                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC6(unsigned int config)
{
    _OC6IF = 0;                     /* Clear IF bit */
    _OC6IP = (config &0x0007);      /* Assign Interrupt Priority */
    _OC6IE = (config &0x0008)>>3;   /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
