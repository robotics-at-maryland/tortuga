/**
 * @file BlackfinBlockDFT.cpp
 * @author Leo Singer
 *
 * @see http://docs.blackfin.uclinux.org/doku.php?id=toolchain:libbfdsp
 *      for brief overview of Analog Devices' libbfdsp signal processing library
 * @see http://www.analog.com/static/imported-files/software_manuals/50_blackfin_cc.rev5.1.pdf
 *      for ADI's own documentation on libbfdsp
 *
 */


#include "BlackfinBlockDFT.h"
#include <filter.h>

BlackfinBlockDFT::BlackfinBlockDFT()
{
    // Generate twiddle factor table for FFT
    twidfftrad4_fr16(fftTwid, BLOCKSIZE);
}

BlackfinBlockDFT::~BlackfinBlockDFT()
{
    /// TODO
}

void BlackfinBlockDFT::readBlock()
{
    /// TODO
}

void BlackfinBlockDFT::doDFT(int channel)
{
    int block_exponent;
    // Fourier transform the current block.
    rfft_fr16(block[channel], dft, rfftTwid, 1, BLOCKSIZE, &block_exponent, 0);
}
