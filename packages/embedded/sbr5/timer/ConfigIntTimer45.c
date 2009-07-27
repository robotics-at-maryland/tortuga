#include <timer.h>

#ifdef _T5IF

/*******************************************************************
*    Function Name:  ConfigIntTimer45                              *
*    Description:    This function configures interrupt and sets   *
*                    Interrupt Priority                            *
*    Parameters:     unsigned int config                           *               
*    Return Value:   None                                          *
*******************************************************************/
void ConfigIntTimer45(unsigned int config)
{
    _T5IF = 0;                   /* clear IF bit */
    _T5IP = (config &0x0007);    /* assigning Interrupt Priority */
    _T5IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

#else
#warning "Does not build on this target"
#endif
