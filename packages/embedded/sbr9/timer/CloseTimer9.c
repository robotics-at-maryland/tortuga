#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  CloseTimer9                                    *
*    Description:    This routine disables the Timer9 and its       *
*                    interrupt enable and flag bits.                *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer9(void)
{
    _T9IE = 0;      /* Disable the Timer9 interrupt */
    T9CONbits.TON = 0;      /* Disable timer9 */
    _T9IF = 0;      /* Clear Timer9 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
