#include <outcompare.h>

#ifdef _OC7IF

/************************************************************************
*    Function Name:  ConfigIntOC7                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC7(unsigned int config)
{
    _OC7IF = 0;                    /* Clear IF bit */
    _OC7IP = (config &0x0007);     /* Assign Interrupt Priority */
    _OC7IE = (config &0x0008)>>3;  /* Enable/disable Interrupt */
}

#else
#warning "Does not build on this target"
#endif
