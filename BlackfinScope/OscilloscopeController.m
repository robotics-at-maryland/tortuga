//
//  OscilloscopeController.m
//  BlackfinScope
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/27/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "OscilloscopeController.h"
#import "scope.h"
#import <Ice/Ice.h>


@implementation OscilloscopeController

- (void)awakeFromNib
{
    communicator = [ICEUtil createCommunicator: 0
                                          argv: nil];
    
    adapter = [communicator createObjectAdapterWithEndpoints: @"oscViewer"
                                                   endpoints: @"default -p 10001"];
    
    id<ICEObjectPrx> selfPrx = [adapter addWithUUID: self];
    [adapter activate];
    
    oscPrx = [ramsonarscopeOscilloscopePrx uncheckedCast:
                [communicator stringToProxy: @"osc:default -p 10000"]];
    
    [oscPrx SetViewer: (id<ramsonarscopeViewerPrx>) selfPrx];
    
    [oscPrx retain];
}

- (void)NotifyCapture:(ICECurrent*)current {
    NSLog(@"NotifyCapture");
    ramsonarscopeOscilloscopeCapture* capture = [oscPrx GetLastCapture];
    [triggerModeControl setSelectedSegment: [capture newTriggerMode]];
    view.lastCapture = capture;
    [view setNeedsDisplay:YES];
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

- (IBAction)horizontalZoomChanged: (id)sender
{
    [oscPrx SetHorizontalZoom: [sender floatValue]];
}

@end
