#include <outcompare.h>

#ifdef _OC3IF

/************************************************************************
*    Function Name:  ConfigIntOC3                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC3(unsigned int config)
{
    _OC3IF = 0;                    /* Clear IF bit */
    _OC3IP = (config &0x0007);     /* Assign Interrupt Priority */
    _OC3IE = (config &0x0008)>>3;  /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
