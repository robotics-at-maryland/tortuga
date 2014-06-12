#include <outcompare.h>

#ifdef _OC4IF

/********************************************************************
*    Function Name:  SetDCOC4PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC4PWM(unsigned int dutycycle)
{   
    
    /* check OC is in PWM Mode.*/
    if((OC4CONbits.OCM & 0x06) == 0x06) 
       OC4RS = dutycycle; /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
