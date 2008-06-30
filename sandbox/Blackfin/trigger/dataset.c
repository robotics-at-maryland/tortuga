#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"


struct dataset * createDataset(int size)
{
    if(size < 0)
    {
        fprintf(stderr, "Cannot create dataset of negative size\n");
        return NULL;
    }

    if(MAX_SEGMENTS * ALLOC_UNIT_SIZE < size)
    {
        fprintf(stderr, "Requested dataset too large for current allocation unit size and count\n");
        return NULL;
    }


    struct dataset * s = malloc(sizeof(struct dataset));
    memset(s, 0, sizeof(struct dataset));

    int i, j;
    fprintf(stderr, "Using allocation units of %d samples\n", ALLOC_UNIT_SIZE);
    int numUnits = size / ALLOC_UNIT_SIZE;
    if(size > numUnits * ALLOC_UNIT_SIZE)
        numUnits++;

    for(i=0; i<numUnits; i++)
    {
        for(j=0; j<4; j++)
        {
            s->data[i][j] = malloc(ALLOC_UNIT_SIZE * sizeof(signed short));
            if(s->data[i][j] == NULL)
            {
                fprintf(stderr, "Could not allocate inot %d for channel %d\nEPIC FAIL\n", i, j);
                exit(-1);
            }
        }
    }

    s->size = size;
    s->numUnits = numUnits;

    return s;
}


int destroyDataset(struct dataset * s)
{
    if(s == NULL)
        return -1;

    int i, j;
    for(i=0; i<s->numUnits; i++)
        for(j=0; j<4; j++)
            free(s->data[i][j]);

    free(s);
    return 0;
}

signed short getSample(struct dataset* s, int ch, int index)
{
    if(s == NULL)
    {
        fprintf(stderr, "Bad dataset!\n");
        return -1;
    }

    if(ch < 0 || ch > 3)
    {
        fprintf(stderr, "Bad channel number: %d\n", ch);
        return -1;
    }

    if(index < 0 || index >= s->size)
    {
        fprintf(stderr, "Sample index out of range! Max: %d\n, requested: %d\n", s->size-1, index);
        return -1;
    }
    int unit = index >> ALLOC_UNIT_NUMBITS;
    int offset = index & ALLOC_UNIT_MASK;

    return *(s->data[unit][ch]+offset);
}

int putSample(struct dataset* s, int ch, int index, signed short value)
{
    if(s == NULL)
    {
        fprintf(stderr, "Bad dataset!\n");
        return -1;
    }

    if(ch < 0 || ch > 3)
    {
        fprintf(stderr, "Bad channel number: %d\n", ch);
        return -1;
    }

    if(index < 0 || index >= s->size)
    {
        fprintf(stderr, "Sample index out of range! Max: %d\n, requested: %d\n", s->size-1, index);
        return -1;
    }
    int unit = index >> ALLOC_UNIT_NUMBITS;
    int offset = index & ALLOC_UNIT_MASK;

    *(s->data[unit][ch]+offset) = value;
}
