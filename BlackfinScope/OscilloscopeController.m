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
    @try {
        communicator = [ICEUtil createCommunicator: 0
                                              argv: nil];
        
        adapter = [communicator createObjectAdapterWithEndpoints: @"oscViewer"
                                                       endpoints: @"default -p 10001"];
        
        id<ICEObjectPrx> selfPrx = [adapter addWithUUID: self];
        [adapter activate];
        
        oscPrx = [ramsonarscopeOscilloscopePrx uncheckedCast:
                  //[communicator stringToProxy: @"osc:default -h localhost -p 10000"]];
                  [communicator stringToProxy: @"osc:default -h 192.168.10.19 -p 10000"]];
        
        [oscPrx SetViewer: (id<ramsonarscopeViewerPrx>) selfPrx];
        [oscPrx SetScopeMode: ramsonarscopeScopeModeOscilloscope];
        
        [oscPrx SetTriggerMode: ramsonarscopeTriggerModeStop];
        [oscPrx SetTriggerSlope: ramsonarscopeTriggerSlopeRising];
        [oscPrx SetTriggerLevel: 0];
        [oscPrx SetHorizontalZoom: 0];
        
        [oscPrx retain];
    } @catch (NSException* ex) {
        NSRunCriticalAlertPanel([ex name],
                                [ex reason] , @"Quit", nil, nil);
        [NSApp terminate];
    }
}

- (void)NotifyCapture:(ICECurrent*)current {
    ramsonarscopeOscilloscopeCapture* capture = [oscPrx GetLastCapture];
    [triggerModeControl setSelectedSegment: [capture newTriggerMode]];
    view.lastCapture = capture;
    [view setNeedsDisplay: YES];
    [triggerProgressIndicator stopAnimation: self];
}

- (IBAction)scopeModeChanged: (id)sender
{
    ramsonarscopeScopeMode scopeMode = [sender selectedSegment];
    [oscPrx SetScopeMode: scopeMode];
    view->scopeMode = scopeMode;
    
    if (scopeMode == ramsonarscopeScopeModeSpectrumAnalyzer)
        [self updateSpectrum];
    
    [view setNeedsDisplay: YES];
}

- (IBAction)triggerLevelChanged: (id)sender
{
    short val = [sender floatValue]/10 * 300 * (1 << view->verticalSensitivity[view->triggerChannel]);
    [oscPrx SetTriggerLevel: val];
    view->triggerLevel = val;
    [view setNeedsDisplay: YES];
}

- (IBAction)triggerChannelChanged: (id)sender
{
    short val = [sender selectedSegment];
    [oscPrx SetTriggerChannel: val];
    view->triggerChannel = val;
    [view setNeedsDisplay: YES];
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
    [NSApp terminate: self];
}

- (void) updateSpectrum
{
    if (view->scopeMode == ramsonarscopeScopeModeSpectrumAnalyzer)
    {
        view.lastSpectrum = [oscPrx GetLastSpectrum];
        [view setNeedsDisplay: YES];
        [self performSelector: @selector(updateSpectrum)
                   withObject: nil
                   afterDelay: 1/20.0];
    }
}

@end
