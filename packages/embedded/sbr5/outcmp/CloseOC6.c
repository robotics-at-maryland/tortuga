#include <outcompare.h>

#ifdef _OC6IF

/*********************************************************************
* Function Name :  CloseOC6                                          *
* Description   :  This routine disables the Output Compare and its  *
*                  interrupt bits.                                   *
* Parameter     :  None                                              *
* Return Value  :  None                                              *
*********************************************************************/

void CloseOC6()
{   
    _OC6IE = 0;/* Disable the Interrupt bit in IEC Register */
    OC6CONbits.OCM = 0;/* Turn off Output Compare6 */
    _OC6IF = 0;/* Disable the Interrupt Flag bit in IFS Register */
}

#else
#warning "Does not build on this target"
#endif
