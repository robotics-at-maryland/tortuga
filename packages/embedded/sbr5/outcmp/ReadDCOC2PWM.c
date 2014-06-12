#include <outcompare.h>

#ifdef _OC2IF

/***********************************************************************
*    Function Name:  ReadDCOC2PWM                                      *
*    Description:    This routine reads duty cycle from Secondary      *
*                    register in PWM mode                              *
*    Parameters:     None                                              *
*    Return Value:   unsigned int: duty cycle from Secondary register  *
***********************************************************************/

unsigned int ReadDCOC2PWM()
{
    /* check if OC is in PWM Mode */
    if((OC2CONbits.OCM & 0x06) == 0x06)
    {       
        return OC2RS; /* Output Compare Secondary Register */
    }
    else 
        return -1;
}

#else
#warning "Does not build on this target"
#endif
