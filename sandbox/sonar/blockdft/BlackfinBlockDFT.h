/**
 * @file BlackfinBlockDFT.h
 * @author Leo Singer
 *
 */

#ifndef _RAM_SONAR_BLACKFINBLOCKDFT_H
#define _RAM_SONAR_BLACKFINBLOCKDFT_H

#include "blockdft.h"

#include <fract_complex.h>
#include <fract_math.h>

class BlackfinBlockDFT : public BlockDFT {
private:
    complex_fract16 rfftTwid[BLOCKSIZE];
public:
    BlackfinBlockDFT();
    virtual ~BlackfinBlockDFT();
    virtual void readBlock();
    virtual void doDFT(int channel);
};

#endif
