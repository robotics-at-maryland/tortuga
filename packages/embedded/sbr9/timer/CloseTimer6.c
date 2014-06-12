#include <timer.h>

#ifdef _T6IF

/********************************************************************
*    Function Name:  CloseTimer6                                    *
*    Description:    This routine disables the Timer6 and its       *
*                    interrupt enable and flag bits.                *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer6(void)
{
    _T6IE = 0;      /* Disable the Timer6 interrupt */
    T6CONbits.TON = 0;      /* Disable timer6 */
    _T6IF = 0;      /* Clear Timer6 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
