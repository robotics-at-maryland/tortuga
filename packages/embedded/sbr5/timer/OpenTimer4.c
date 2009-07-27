#include <timer.h>

#ifdef _T4IF

/********************************************************************
*    Function Name:  OpenTimer4                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer4    *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer4(unsigned int config,unsigned int period)
{
    TMR4  = 0;          /* Reset Timer4 to 0x0000 */
    PR4   = period;     /* assigning Period to Timer period register */
    T4CON = config;     /* configure timer control reg */
    T4CONbits.T32 = 0;
}

#else
#warning "Does not build on this target"
#endif
