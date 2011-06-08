#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  OpenTimer5                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer5    *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer5(unsigned int  config,unsigned int period)
{
    TMR5  = 0;          /* Reset Timer5 to 0x0000 */
    PR5   = period;     /* Assigning Period to Timer period register.*/
    T5CON =  config;    /* configure timer control reg */
}

#else
#warning "Does not build on this target"
#endif
