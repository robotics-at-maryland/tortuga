#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  OpenTimer45                                    *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer4    *
*                    and timer5 period to timer period register     *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer45(unsigned int  config,unsigned long period)
{
    TMR4 = 0;               /* Reset Timer4 to 0x0000 */
    TMR5 = 0;               /* Reset Timer5 to 0x0000 */
    PR4 = period;           /* assigning Period to Timer period register */
    PR5 = period>>16;       /* Period to PR4 and PR5 Register */
    T4CON = config;         /* configure timer control reg */
    T4CONbits.T32 = 1;
}

#else
#warning "Does not build on this target"
#endif
