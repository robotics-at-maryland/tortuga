#include <outcompare.h>

#ifdef _OC6IF

/********************************************************************
*    Function Name:  SetDCOC6PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC6PWM(unsigned int dutycycle)
{   
     /* check OC is in PWM Mode.*/
    if((OC6CONbits.OCM & 0x06) == 0x06) 
       OC6RS = dutycycle; /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
