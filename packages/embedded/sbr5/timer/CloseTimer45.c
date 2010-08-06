#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  CloseTimer45                                   *
*    Description:    This routine disables the Timer4 and 5 and its *
*                    interrupt and flag bits                        *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer45(void)
{
    _T5IE = 0;      /* Disable the Timer5 interrupt */
    if (T4CONbits.T32 == 1)
        T4CONbits.TON = 0;  /* Disable Timer 4 */
    _T5IF = 0;      /* Clear Timer5 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
