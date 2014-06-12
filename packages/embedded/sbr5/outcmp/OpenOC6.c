#include <outcompare.h>

#ifdef _OC6IF

/*****************************************************************************
*    Function Name:  OpenOC6                                                 *
*    Description:    This routine configures output compare module and loads *
*                    the compare registers                                   *
*    Parameters  :   unsigned int config, unsigned int value1,               *
*                    unsigned int value2                                     *
*    Return Value:   None                                                    *
*    Notes           value1 is the compare value for the OCRS register       *
*                    value2 is the compare value for the OCR register        *
*****************************************************************************/

void OpenOC6(unsigned int config, unsigned int value1, unsigned int value2)
{
    OC6CONbits.OCM = 0; /* turn off OC before switching to new mode */
    OC6RS = value1;     /* assign value1 to OCxRS Secondary Register */
    OC6R = value2;      /* assign value2 to OCxR Main Register*/  
    OC6CON = config;    /* assign config to OCxCON Register*/
}

#else
#warning "Does not build on this target"
#endif

