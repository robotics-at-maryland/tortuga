#include <outcompare.h>

#ifdef _OC7IF

/********************************************************************
*    Function Name:  SetDCOC7PWM                                    *
*    Description:    This routine writes the duty cycle OCRS reg    *
*    Parameters:     unsigned int duty cycle                        *
*    Return Value:   None                                           *
********************************************************************/

void  SetDCOC7PWM(unsigned int dutycycle)
{   
    /* check OC is in PWM Mode.*/
    if((OC7CONbits.OCM & 0x06) == 0x06) 
       OC7RS = dutycycle; /* assign to OCRS */
}

#else
#warning "Does not build on this target"
#endif
