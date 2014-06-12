/**
 * @file DesktopBlockDFT.h
 * @author Leo Singer
 *
 */

#ifndef _RAM_SONAR_DESKTOPBLOCKDFT_H
#define _RAM_SONAR_DESKTOPBLOCKDFT_H

#include "blockdft.h"
#include <fftw3.h>

class DesktopBlockDFT : public BlockDFT {
private:
    fftw_plan fftwPlan;
    double fftwIn[BLOCKSIZE];
    fftw_complex fftwOut[BLOCKSIZE];
public:
    DesktopBlockDFT();
    virtual ~DesktopBlockDFT();
    virtual void readBlock();
    virtual void doDFT(int channel);
};

#endif
