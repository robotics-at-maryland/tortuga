//
//  OscilloscopeController.m
//  BlackfinScope
//
//  Created by Leo Singer on 7/27/09.
//  Copyright 2009 Robotics@Maryland. All rights reserved.
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
                [communicator stringToProxy: @"osc:default -h 192.168.10.19 -p 10000"]];
    
    [oscPrx SetViewer: (id<ramsonarscopeViewerPrx>) selfPrx];
    
    [oscPrx SetTriggerMode:ramsonarscopeTriggerModeStop];
    [oscPrx SetTriggerSlope:ramsonarscopeTriggerSlopeRising];
    [oscPrx SetTriggerLevel:0];
    [oscPrx SetHorizontalZoom:0];
    
    [oscPrx retain];
}

- (void)NotifyCapture:(ICECurrent*)current {
    NSLog(@"NotifyCapture");
    ramsonarscopeOscilloscopeCapture* capture = [oscPrx GetLastCapture];
    [triggerModeControl setSelectedSegment: [capture newTriggerMode]];
    view.lastCapture = capture;
    [view setNeedsDisplay:YES];
    [triggerProgressIndicator stopAnimation: self];
}

- (IBAction)triggerLevelChanged: (id)sender
{
    short val = [sender floatValue]/10 * 300 * (1 << view->verticalSensitivity[view->triggerChannel]);
    [oscPrx SetTriggerLevel:val];
    view->triggerLevel = val;
    [view setNeedsDisplay:YES];
}

- (IBAction)triggerChannelChanged: (id)sender
{
    short val = [sender selectedSegment];
    [oscPrx SetTriggerChannel: val];
    view->triggerChannel = val;
    [view setNeedsDisplay:YES];
}

- (IBAction)triggerModeChanged: (id)sender
{
    if ([sender selectedSegment] == ramsonarscopeTriggerModeRun)
        [triggerProgressIndicator startAnimation: self];
    
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

- (void)windowWillClose:(NSNotification *)aNotification {
    [NSApp terminate:self];
}

@end
