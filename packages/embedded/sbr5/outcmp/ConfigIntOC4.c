#include <outcompare.h>

#ifdef _OC4IF

/************************************************************************
*    Function Name:  ConfigIntOC4                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC4(unsigned int config)
{
    _OC4IF = 0;                    /* Clear IF bit */
    _OC4IP = (config &0x0007);     /* Assign Interrupt Priority */ 
    _OC4IE = (config &0x0008)>> 3; /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
