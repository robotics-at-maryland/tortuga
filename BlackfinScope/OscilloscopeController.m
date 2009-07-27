//
//  OscilloscopeController.m
//  BlackfinScope
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/27/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "OscilloscopeController.h"


@implementation OscilloscopeController

- (void)NotifyCapture:(ICECurrent*)current {
    ramsonarscopeOscilloscopeCapture* capture = [oscPrx GetLastCapture];
}

- (IBAction)triggerChannelChanged: (id)sender
{
    [oscPrx SetTriggerChannel: [sender selectedSegment]];
}

- (IBAction)triggerModeChanged: (id)sender
{
    [oscPrx SetTriggerMode: [sender selectedSegment]];
}

- (IBAction)triggerSlopeChanged: (id)sender
{
    [oscPrx SetTriggerSlope: [sender selectedSegment]];
}

@end
