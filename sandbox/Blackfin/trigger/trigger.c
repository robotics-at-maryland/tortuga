#include <stdio.h>
#include <stdlib.h>
#include "trigger.h"
#include "dataset.h"

int blockTrigger(struct dataset * s, int ch, int highestDftSample)
{
    if(s == NULL)
        return -1;

    if(highestDftSample < BACKTRACK)
    {
        fprintf(stderr, "DFT trigger point too close to left edge of ping.\n");
        return -1;
    }

    fprintf(stderr, "Beginning block triggering using %d blocks at sample %d\n",
                    BACKTRACK / BLOCKSIZE, highestDftSample);

    int i=0, j=0;
    int searchStart, searchEnd;

    searchStart = highestDftSample - BACKTRACK;
    searchEnd = highestDftSample;

    int blockMax = 0;
    int lastBlockMax = 0;

    int blockJump = 0;
    int highestJump = 0;
    int highestJumpIndex = 0;

    int curBlock=0;
    int triggeredBlock = 0;


    fprintf(stdout, "Searching region %d to %d\n", searchStart, searchEnd);

    for(i=searchStart; i<searchEnd; i+= BLOCKSIZE)
    {
        lastBlockMax = blockMax;
        blockMax = getSample(s, ch, i+j);

        /* Find maximum of this block */
        for(j=0; j<BLOCKSIZE; j++)
        {
            signed short curSample = getSample(s, ch, i+j);
            if(sAbs(curSample) > blockMax)
                blockMax = curSample;
        }

        blockJump = abs(blockMax - lastBlockMax);

        if(blockJump > highestJump)
        {
            highestJump = blockJump;
            highestJumpIndex = i;
            triggeredBlock = curBlock;
        }
        curBlock++;
    }

    fprintf(stderr, "Highest block jump occurs at sample %d\n", highestJumpIndex);
    fprintf(stderr, "Triggered on block %d\n", triggeredBlock);


    return highestJumpIndex;
}


signed short sAbs(signed short v)
{
    return v > 0 ? v : -v;
}
