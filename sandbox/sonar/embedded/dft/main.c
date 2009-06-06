#include "dft.h"

int main(void)
{
	int nchannels = 4, k = 10, N = 100;
	int niters = 1000; /* number of iterations */
	sliding_dft_t *dft;
	adcdata_t zerosamp[nchannels];
	int i;
	
	/* As a simple, stupid test, we'll feed the DFT a bunch of zeros every time */
	for (i = 0 ; i < nchannels ; i ++)
		zerosamp[i] = 0;
	
	dft = dft_alloc(nchannels, k, N);
	
	for (i = 0 ; i < niters ; i ++)
		dft_update(dft, zerosamp);
	
	dft_free(dft);
}

