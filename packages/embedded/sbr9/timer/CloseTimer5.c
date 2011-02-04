#include <timer.h>

#ifdef _T5IF

/********************************************************************
*    Function Name:  CloseTimer5                                    *
*    Description:    This routine disables the Timer5 and its       *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer5(void)
{
    _T5IE = 0;  /* Disable the Timer5 interrupt */
    T5CONbits.TON=0;    /* disable Timer 5 */
    _T5IF = 0;  /* Clear Timer5 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif
