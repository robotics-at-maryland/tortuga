#include <timer.h>

#ifdef _T6IF

/********************************************************************
*    Function Name:  OpenTimer6                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer6    *
*                    period: Value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer6(unsigned int config,unsigned int period)
{
    TMR6  = 0;          /* Reset Timer6 to 0x0000 */
    PR6   = period;     /* assigning Period to Timer period register */
    T6CON = config;     /* Configure timer control reg */
}

#else
#warning "Does not build on this target"
#endif
