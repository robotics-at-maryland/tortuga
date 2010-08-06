#include <outcompare.h>

#ifdef _OC1IF

/************************************************************************
*    Function Name:  ConfigIntOC1                                	*
*    Description:    This Function Configures Interrupt and sets        *
*                    Interrupt Priority                                 *
*    Parameters:     unsigned int config                                *
*    Return Value:   None                                               *
************************************************************************/

void ConfigIntOC1(unsigned int config)
{   
    _OC1IF = 0 ;                  /* Clear IF bit */
    _OC1IP = (config &0x0007);    /* Assign Interrupt Priority */ 
    _OC1IE = (config &0x0008)>>3; /* Enable/disable Interrupt */ 
}

#else
#warning "Does not build on this target"
#endif
