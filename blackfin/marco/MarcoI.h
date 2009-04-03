/*
 *  MarcoI.h
 *  polo
 *
 *  Created by auvsi on 4/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MARCOI_H
#define _MARCOI_H

#include <marcopolo.h>

class MarcoI : public ram::marcopolo::Marco
{
private:
	int triggerChannel;
	short triggerThreshold;
	ram::marcopolo::PoloPrx polo;
public:
	MarcoI();
	void go();
	virtual void setChannelEnabled(Ice::Int channel, bool enabled, const Ice::Current&);
	virtual bool getChannelEnabled(Ice::Int channel, const Ice::Current&);
	virtual void setTrigger(Ice::Int channel, Ice::Int level, const Ice::Current&);
	virtual void registerPolo(const ram::marcopolo::PoloPrx&, const Ice::Current&);
};

#endif
