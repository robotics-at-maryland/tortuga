#include <outcompare.h>

#ifdef _OC3IF

/********************************************************************
*    Function Name:  SetDCOC3PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC3PWM(unsigned int dutycycle)
{   
    
    /* check OC is in PWM Mode */
    if((OC3CONbits.OCM & 0x06) == 0x06) 
       OC3RS = dutycycle;  /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
