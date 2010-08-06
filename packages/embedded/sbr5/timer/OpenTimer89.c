#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  OpenTimer89                                    *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer8  *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer89(unsigned int  config,unsigned long period)
{
    TMR8 = 0;               /* Reset Timer8 to 0x0000 */
    TMR9 = 0;               /* Reset Timer9 to 0x0000 */
    PR8 = period;           /* assigning Period to PR8 */
    PR9 = period>>16;       /* Period to  PR9Register */
    T8CON =  config;        /* Configure timer control reg */
    T8CONbits.T32 = 1;
}

#else
#warning "Does not build on this target"
#endif
