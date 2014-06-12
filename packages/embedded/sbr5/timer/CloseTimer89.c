#include <timer.h>

#ifdef _T9IF

/********************************************************************
*    Function Name:  CloseTimer89                                   *
*    Description:    This routine disables the Timer8 and 9 and its *
*                    interrupt and flag bits                        *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer89(void)
{
    _T9IE = 0;      /* Disable the Timer9 interrupt */
    if (T8CONbits.T32 == 1)
        T8CONbits.TON = 0;  /* Disable Timer 8 */
    _T9IF = 0;      /* Clear Timer5 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
