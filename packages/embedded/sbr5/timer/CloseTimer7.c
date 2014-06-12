#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  CloseTimer7                                    *
*    Description:    This routine disables the Timer7 and its       *
*                    interrupt enable and flag bits.                *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer7(void)
{
    _T7IE = 0;      /* Disable the Timer7 interrupt */
    T7CONbits.TON = 0;      /* Disable timer7 */
    _T7IF = 0;      /* Clear Timer interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
