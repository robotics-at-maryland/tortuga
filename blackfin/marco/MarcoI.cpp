/*
 *  MarcoI.cpp
 *  polo
 *
 *  Created by auvsi on 4/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MarcoI.h"
#include "fpga.h"

static Ice::Short snapshot[480][4];
static ram::marcopolo::Snapshot snap;

MarcoI::MarcoI() : triggerChannel(0), triggerThreshold(0)
{
	snap.samples = ram::marcopolo::ShortList(480*4);
}

void MarcoI::go()
{
	bool triggered = false;
	int snapshotIndex = 0;
	while (true)
	{
		short numToCopy = VAR_FIFO_COUNT1A;
		while (numToCopy > 0)
		{
			Ice::Short* sample = snapshot[snapshotIndex];
			sample[0] = VAR_FIFO_DMA1A;
			sample[1] = VAR_FIFO_DMA1B;
			sample[2] = VAR_FIFO_DMA2A;
			sample[3] = VAR_FIFO_DMA2B;
			
			if (triggered)
			{
				if (++snapshotIndex >= 480)
				{
					triggered = false;
					snapshotIndex = 0;
					
					if (polo)
					{
						for (int i = 0 ; i < 480 * 4 ; i ++)
							snap.samples[i] = snapshot[0][i];
						polo->reportData(snap);
					}
				}
			} else {
				short triggerValue = sample[triggerChannel];
				if (triggerValue >= triggerThreshold)
				{
					triggered = true;
					snapshotIndex = 0;
				}
			}
		}
	}
}

void MarcoI::setChannelEnabled(Ice::Int channel, bool enabled, const Ice::Current&)
{
	/// TODO:
}

bool MarcoI::getChannelEnabled(Ice::Int channel, const Ice::Current&)
{
	/// TODO:
	return false;
}

void MarcoI::setTrigger(Ice::Int channel, Ice::Int level, const Ice::Current&)
{
	if (channel >= 0 && channel < 4)
	{
		triggerChannel = channel;
		triggerThreshold = level;
	}
}

void MarcoI::registerPolo(const ram::marcopolo::PoloPrx& poloPrx, const Ice::Current&)
{
	polo = poloPrx;
	go();
}
