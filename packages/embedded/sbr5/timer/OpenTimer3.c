#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  OpenTimer3                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer3    *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer3(unsigned int  config,unsigned int period)
{
    TMR3  = 0;      /* Reset Timer3 to 0x0000 */
    PR3   = period; /* assigning Period to Timer period register.*/
    T3CON = config; /* configure timer control reg */
}

#else
#warning "Does not build on this target"
#endif
