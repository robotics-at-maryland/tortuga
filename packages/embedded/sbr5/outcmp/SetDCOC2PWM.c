#include <outcompare.h>

#ifdef _OC2IF

/********************************************************************
*    Function Name:  SetDCOC2PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC2PWM(unsigned int dutycycle)
{       
    /* check OC is in PWM Mode */
    if((OC2CONbits.OCM & 0x06) == 0x06) 
       OC2RS = dutycycle; /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
