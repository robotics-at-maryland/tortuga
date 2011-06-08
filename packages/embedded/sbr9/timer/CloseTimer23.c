#include <timer.h>

#ifdef _T3IF

/********************************************************************
*    Function Name:  CloseTimer23                                   *
*    Description:    This routine disables the Timer2 and 3 and its *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer23(void)
{
    _T3IE = 0;      /* Disable the Timer3 interrupt */
    if (T2CONbits.T32 == 1)
        T2CONbits.TON = 0;  /* Disable Timer2 */
    _T3IF = 0;      /* Clear Timer3 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif

