/**
 * @file main.cpp
 * @author Leo Singer
 *
 */

#include "DesktopBlockDFT.h"

int main (int argc, char * const argv[]) {
    static DesktopBlockDFT blockDFT;
    
    while (1)
        blockDFT.processBlock();
    
    return 0;
}
