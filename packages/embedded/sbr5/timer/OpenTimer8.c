#include <timer.h>

#ifdef _T8IF

/********************************************************************
*    Function Name:  OpenTimer8                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer8    *
*                    period: Value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer8(unsigned int config,unsigned int period)
{
    TMR8  = 0;          /* Reset Timer8 to 0x0000 */
    PR8   = period;     /* assigning Period to Timer period register */
    T8CON = config;     /* Configure timer control reg */
}

#else
#warning "Does not build on this target"
#endif
