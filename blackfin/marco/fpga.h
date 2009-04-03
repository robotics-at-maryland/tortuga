#ifndef _FPGA_H
#define _FPGA_H

#include "addresses.h"

#define PTR_FIFO_DMA1A			((short*)ADDR_FIFO_DMA1A)
#define PTR_FIFO_DMA1B			((short*)ADDR_FIFO_DMA1B)
#define PTR_FIFO_DMA2A			((short*)ADDR_FIFO_DMA2A)
#define PTR_FIFO_DMA2B			((short*)ADDR_FIFO_DMA2B)

#define VAR_FIFO_DMA1A			(*PTR_FIFO_DMA1A)
#define VAR_FIFO_DMA1B			(*PTR_FIFO_DMA1B)
#define VAR_FIFO_DMA2A			(*PTR_FIFO_DMA2A)
#define VAR_FIFO_DMA2B			(*PTR_FIFO_DMA2B)

#define PTR_FIFO_COUNT1A		((short*)ADDR_FIFO_COUNT1A)
#define PTR_FIFO_COUNT1B		((short*)ADDR_FIFO_COUNT1B)
#define PTR_FIFO_COUNT2A		((short*)ADDR_FIFO_COUNT2A)
#define PTR_FIFO_COUNT2B		((short*)ADDR_FIFO_COUNT2B)

#define VAR_FIFO_COUNT1A		(*PTR_FIFO_COUNT1A)
#define VAR_FIFO_COUNT1B		(*PTR_FIFO_COUNT1B)
#define VAR_FIFO_COUNT2A		(*PTR_FIFO_COUNT2A)
#define VAR_FIFO_COUNT2B		(*PTR_FIFO_COUNT2B)

#endif
