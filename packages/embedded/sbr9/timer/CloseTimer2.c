#include <timer.h>

#ifdef _T2IF

/********************************************************************
*    Function Name:  CloseTimer2                                    *
*    Description:    This routine disables the Timer2 and its       *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer2(void)
{
    _T2IE = 0;      /* Disable the Timer2 interrupt */
    T2CONbits.TON = 0;      /* Disable timer2 */
    _T2IF = 0;      /* Clear Timer interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
