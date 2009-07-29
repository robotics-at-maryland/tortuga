/**
 * @file main.cpp
 * @author Leo Singer
 *
 */

#include "DesktopBlockDFT.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char * const argv[]) {
    
    if (argc > 1)
    {
        FILE* fp;
        if ((fp = fopen(argv[1], "rb")) == NULL)
        {
            perror("failed to open input file");
            return EXIT_FAILURE;
        }
        stdin = fp;
    }
    
    static DesktopBlockDFT blockDFT;
    
    while (1)
        blockDFT.processBlock();
    
    return 0;
}
