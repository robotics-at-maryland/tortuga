#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  OpenTimer23                                    *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer2    *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer23(unsigned int  config,unsigned long period)
{
    TMR2 = 0;               /* Reset Timer2 to 0x0000 */
    TMR3 = 0;               /* Reset Timer3 to 0x0000 */
    PR2 = period;           /* assigning Period to PR2 */
    PR3 = period>>16;       /* Period to  PR3 Register */
    T2CON =  config;        /* Configure timer control reg */
    T2CONbits.T32 = 1;
}

#else
#warning "Does not build on this target"
#endif
