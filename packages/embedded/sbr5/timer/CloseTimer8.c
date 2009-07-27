#include <timer.h>

#ifdef _T8IF

/********************************************************************
*    Function Name:  CloseTimer8                                    *
*    Description:    This routine disables the Timer8 and its       *
*                    interrupt enable and flag bits.                *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer8(void)
{
    _T8IE = 0;      /* Disable the Timer8 interrupt */
    T8CONbits.TON = 0;      /* Disable timer8 */
    _T8IF = 0;      /* Clear Timer8 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
