#include <outcompare.h>

#ifdef _OC8IF

/********************************************************************
*    Function Name:  SetDCOC8PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC8PWM(unsigned int dutycycle)
{   
    /* check OC is in PWM Mode.*/
    if((OC8CONbits.OCM & 0x06) == 0x06) 
       OC8RS = dutycycle; /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
