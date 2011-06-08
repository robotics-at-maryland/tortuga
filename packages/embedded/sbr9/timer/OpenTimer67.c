#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  OpenTimer67                                    *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer6    *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer67(unsigned int  config,unsigned long period)
{
    TMR6 = 0;               /* Reset Timer6 to 0x0000 */
    TMR7 = 0;               /* Reset Timer7 to 0x0000 */
    PR6 = period;           /* assigning Period to PR6*/
    PR7 = period>>16;       /* Period to  PR7Register */
    T6CON =  config;        /* Configure timer control reg */
    T6CONbits.T32 = 1;
}

#else
#warning "Does not build on this target"
#endif
