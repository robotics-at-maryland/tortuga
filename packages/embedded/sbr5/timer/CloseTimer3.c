#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  CloseTimer3                                    *
*    Description:    This routine disables the Timer3 and its       *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer3(void)
{
    _T3IE = 0;      /* Disable the Timer3 interrupt */
    T3CONbits.TON = 0;      /* Disable timer3 */
    _T3IF = 0;      /* Clear Timer interrupt flag */
}

#else 
#warning "Does not build on this target"
#endif
