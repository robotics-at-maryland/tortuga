#ifndef QLZ_HEADER
#define QLZ_HEADER

// Copyright (C) 2006-2007 Lasse Mikkel Reinhold
// lar@quicklz.com
//
// QuickLZ can be used for free under the GPL-1 or GPL-2 license (where anything 
// released into public must be open source) or under a commercial license if such 
// has been acquired (see http://www.quicklz.com/order.html). The commercial license 
// does not cover derived or ported versions created by third parties under GPL.

// User settings
#define QLZ_COMPRESSION_LEVEL 1
//#define QLZ_COMPRESSION_LEVEL 2
//#define QLZ_COMPRESSION_LEVEL 3

#define QLZ_STREAMING_BUFFER 0
//#define QLZ_STREAMING_BUFFER 100000
//#define QLZ_STREAMING_BUFFER 1000000

//#define QLZ_MEMORY_SAFE

// Version 1.40 beta 6. Negative revision means beta.
#define QLZ_VERSION_MAJOR 1
#define QLZ_VERSION_MINOR 4
#define QLZ_VERSION_REVISION (-6)

// Using size_t, memset() and memcpy()
#include <string.h>

// Public functions of QuickLZ
size_t qlz_size_decompressed(const char *source);
size_t qlz_size_compressed(const char *source);
size_t qlz_decompress(const char *source, void *destination, char *scratch_decompress);
size_t qlz_compress(const void *source, char *destination, size_t size, char *scratch_compress);
int qlz_get_setting(int setting);

// Verify compression level
#if QLZ_COMPRESSION_LEVEL != 1 && QLZ_COMPRESSION_LEVEL != 2 && QLZ_COMPRESSION_LEVEL != 3
#error QLZ_COMPRESSION_LEVEL must be 1, 2 or 3
#endif

// Compute QLZ_SCRATCH_COMPRESS and QLZ_SCRATCH_DECOMPRESS
#if QLZ_COMPRESSION_LEVEL == 1
#define QLZ_POINTERS 1
#define QLZ_HASH_VALUES 4096
#elif QLZ_COMPRESSION_LEVEL == 2
#define QLZ_POINTERS 4
#define QLZ_HASH_VALUES 2048
#elif QLZ_COMPRESSION_LEVEL == 3
#define QLZ_POINTERS 16
#define QLZ_HASH_VALUES 4096
#endif

typedef struct 
{
#if QLZ_COMPRESSION_LEVEL == 1
	unsigned int cache[QLZ_POINTERS];
#endif
	const unsigned char *offset[QLZ_POINTERS];
} qlz_hash_entry;

#define QLZ_ALIGNMENT_PADD 8
#define QLZ_BUFFER_COUNTER 8

#define QLZ_SCRATCH_COMPRESS QLZ_ALIGNMENT_PADD + QLZ_BUFFER_COUNTER + QLZ_STREAMING_BUFFER + sizeof(qlz_hash_entry[QLZ_HASH_VALUES]) + QLZ_HASH_VALUES

#if QLZ_COMPRESSION_LEVEL < 3
	#define QLZ_SCRATCH_DECOMPRESS QLZ_ALIGNMENT_PADD + QLZ_BUFFER_COUNTER + QLZ_STREAMING_BUFFER + sizeof(qlz_hash_entry[QLZ_HASH_VALUES]) + QLZ_HASH_VALUES
#else
	#define QLZ_SCRATCH_DECOMPRESS QLZ_ALIGNMENT_PADD + QLZ_BUFFER_COUNTER + QLZ_STREAMING_BUFFER
#endif

#endif




