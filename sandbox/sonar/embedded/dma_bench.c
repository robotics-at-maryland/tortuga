/**
 * @file dma_bench.c
 * Simple comparison of memcpy and dma_memcpy
 * for the Bluetechnix CM-BF637E Blackfin module
 *
 * @author Leo Singer
 */

#include <bfin_sram.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
  const size_t BIG_SIZE = 65535;
  const size_t ITERS    = 100;

  int8_t src[BIG_SIZE];
  int8_t dest[BIG_SIZE];

  int i;

  struct timeval begin, end;

  gettimeofday(&begin, NULL);
  for (i = 0 ; i < ITERS ; i ++)
    memcpy(dest, src, sizeof(*src) * BIG_SIZE);
  gettimeofday(&end, NULL);
  double memcpySeconds = 
    end.tv_sec - begin.tv_sec + 0.000001 * (end.tv_usec - begin.tv_usec);
  
  gettimeofday(&begin, NULL);
  for (i = 0 ; i < ITERS ; i ++)
    dma_memcpy(dest, src, sizeof(*src) * BIG_SIZE);
  gettimeofday(&end, NULL);
  double dmaMemcpySeconds =
    end.tv_sec - begin.tv_sec + 0.000001 * (end.tv_usec - begin.tv_usec);

  double memcpyRate = sizeof(*src) * BIG_SIZE * ITERS / memcpySeconds;
  double dmaMemcpyRate = sizeof(*src) * BIG_SIZE * ITERS / dmaMemcpySeconds;
  
  printf("memcpy (MBps):     %0.23f\n", memcpyRate * 0.000001);
  printf("dma_memcpy (MBps): %0.23f\n", dmaMemcpySeconds * 0.000001);

  return 0;
}
