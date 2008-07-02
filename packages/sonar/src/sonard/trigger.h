#include "dataset.h"

/* Block size, in samples */
/* Should be at least 20 samples or the waves will mess with it */
#define BLOCKSIZE 20

/* How far to backtrack from DFT trigger point? */
/* Large values are OK but need more work */
#define BACKTRACK_BLOCKS   80

/* It is nicer if the backtrack size is a multiple of the block size */
#define BACKTRACK (BACKTRACK_BLOCKS * BLOCKSIZE)

#ifdef __cplusplus
extern "C" {
#endif

/* Given a dataset, a channel, and an index where the DFT is the highest, will
 * return a sample number where the ping begins.
 */
int blockTrigger(struct dataset * s, int ch, int highestDftSample);

signed short sAbs(signed short v);

#ifdef __cplusplus
} // extern "C"
#endif
