#ifndef DATASET_H
#define DATASET_H
#define ALLOC_UNIT_NUMBITS 10

#define ALLOC_UNIT_SIZE (1<<ALLOC_UNIT_NUMBITS)
#define ALLOC_UNIT_MASK ((1<<ALLOC_UNIT_NUMBITS)-1)

#define MAX_SEGMENTS    2550

struct dataset
{
    int size;
    int numUnits;
    signed short * data[MAX_SEGMENTS][4];
};

struct dataset * createDataset(int size);
int destroyDataset(struct dataset* s);

signed short getSample(struct dataset* s, int ch, int index);
int putSample(struct dataset* s, int ch, int index, signed short value);
struct dataset * loadDataset(const char * filename);
#endif
