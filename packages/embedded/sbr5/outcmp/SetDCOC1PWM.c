#include <outcompare.h>

#ifdef _OC1IF

/********************************************************************
*    Function Name:  SetDCOC1PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC1PWM(unsigned int dutycycle)
{   
    /* check if OC is in PWM Mode */
    if((OC1CONbits.OCM & 0x06) == 0x06) 
       OC1RS = dutycycle;   /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif

