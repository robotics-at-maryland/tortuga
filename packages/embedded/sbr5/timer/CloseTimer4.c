#include <timer.h>

#ifdef _T4IF

/********************************************************************
*    Function Name:  CloseTimer4                                    *
*    Description:    This routine disables the Timer4 and its       *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer4(void)
{
    _T4IE = 0;  /* Disable the Timer4 interrupt */
    T4CONbits.TON=0;    /* disable Timer4 */
    _T4IF = 0;  /* Clear Timer4 interrupt flag */
}

#else
#warning "Does not build on this target"
#endif

