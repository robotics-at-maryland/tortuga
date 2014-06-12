#include <timer.h>

#ifdef _T7IF

/********************************************************************
*    Function Name:  CloseTimer67                                  *
*    Description:    This routine disables the Timer6 and 7 and its *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer67(void)
{
    _T7IE = 0;      /* Disable the Timer7 interrupt */
    if (T6CONbits.T32 == 1)
        T6CONbits.TON = 0;  /* Disable Timer6 */
    _T7IF = 0;      /* Clear Timer7 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
