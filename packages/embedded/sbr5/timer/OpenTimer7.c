#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  OpenTimer7                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer7    *
*                    period: Value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer7(unsigned int config,unsigned int period)
{
    TMR7  = 0;          /* Reset Timer7 to 0x0000 */
    PR7   = period;     /* assigning Period to Timer period register */
    T7CON = config;     /* Configure timer control reg */
}

#else
#warning "Does not build on this target"
#endif
