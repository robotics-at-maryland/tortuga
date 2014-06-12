#include <outcompare.h>

#ifdef _OC1IF

/**************************************************************************
* Function Name  :  CloseOC1                                              *
* Description    :  This routine disables the Output Compare and its      *
*                   interrupt bits.                                       *
* Parameter      :  None                                                  *
* Return Value   :  None                                                  *
**************************************************************************/

void CloseOC1()
{   
    _OC1IE = 0;  /* Disable the Interrupt bit in IEC Register */
    OC1CONbits.OCM = 0;  /* Turn off Output Compare1 */
    _OC1IF = 0;  /* Disable the Interrupt Flag bit in IFS Register */
}

#else
#warning "Does not build on this target"
#endif
