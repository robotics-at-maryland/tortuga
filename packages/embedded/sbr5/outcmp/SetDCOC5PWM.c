#include <outcompare.h>

#ifdef _OC5IF

/********************************************************************
*    Function Name:  SetDCOC5PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC5PWM(unsigned int dutycycle)
{   
    /* check OC is in PWM Mode.*/
    if((OC5CONbits.OCM & 0x06) == 0x06) 
       OC5RS = dutycycle; /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
