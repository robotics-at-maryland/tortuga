#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  OpenTimer9                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer9    *
*                    period: Value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer9(unsigned int config,unsigned int period)
{
    TMR9  = 0;          /* Reset Timer9 to 0x0000 */
    PR9   = period;     /* assigning Period to Timer period register */
    T9CON = config;     /* Configure timer control reg */
}

#else
#warning "Does not build on this target"
#endif
