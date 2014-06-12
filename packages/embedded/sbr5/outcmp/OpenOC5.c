#include <outcompare.h>

#ifdef _OC5IF

/*****************************************************************************
*    Function Name:  OpenOC5                                                 *
*    Description:    This routine configures output compare module and loads *
*                    the compare registers                                   *
*    Parameters  :   unsigned int config, unsigned int value1,               *
*                    unsigned int value2                                     *
*    Return Value:   None                                                    *
*    Notes           value1 is the compare value for the OCRS register       *
*                    value2 is the compare value for the OCR register        *
*****************************************************************************/

void OpenOC5(unsigned int config, unsigned int value1, unsigned int value2)
{
    OC5CONbits.OCM = 0; /* turn off OC before switching to new mode */
    OC5RS = value1;     /* assign value1 to OCxRS Secondary Register */
    OC5R = value2;      /* assign value2 to OCxR Main Register*/  
    OC5CON = config;    /* assign config to OCxCON Register*/
}

#else
#warning "Does not build on this target"
#endif

