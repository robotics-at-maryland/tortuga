#include "dataset.h"

/* Block size, in samples */
/* Should be at least 20 samples or the waves will mess with it */
#define BLOCKSIZE 50

/* How far to backtrack from DFT trigger point? */
/* Large values are OK but need more work */
#define BACKTRACK_BLOCKS   40

/* It is nicer if the backtrack size is a multiple of the block size */
#define BACKTRACK (BACKTRACK_BLOCKS * BLOCKSIZE)


/* Given a dataset, a channel, and an index where the DFT is the highest, will
 * return a sample number where the ping begins.
 */
int blockTrigger(struct dataset * s, int ch, int highestDftSample);

signed short sAbs(signed short v);
