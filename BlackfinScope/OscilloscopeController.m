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


@interface OscilloscopeController (PrivateMethods)
@end


@implementation OscilloscopeController

- (void)crashWithException:(NSException *)ex {
    NSAlert* alert = [[[NSAlert alloc] init] autorelease];
    
    [alert addButtonWithTitle:@"Quit"];
    alert.messageText = ex.name;
    alert.informativeText = ex.reason;
    alert.alertStyle = NSCriticalAlertStyle;
    
    [alert beginSheetModalForWindow:window
                      modalDelegate:self
                     didEndSelector:@selector(didEndCrashSheet:returnCode:contextInfo:)
                        contextInfo:NULL];
}

- (void)didEndCrashSheet:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    // Dismiss the alert window.
    [alert.window orderOut:self];
    
    // Close the main window, resulting in the application closing.
    [window close];
}

- (void)awakeFromNib
{
    // Initialize ICE communication subsystem.
    communicator = [[ICEUtil createCommunicator] retain];
    
    // Hook this object up to ICE.
    adapter = [[communicator createObjectAdapterWithEndpoints: @"oscViewer"
                                                   endpoints: @"default -p 10001"] retain];
    [adapter activate];
    selfPrx = [[ramsonarscopeViewerPrx uncheckedCast:[adapter addWithUUID: self]] retain];

    // Register default preference key-value pairs.
    NSDictionary* defaultPreferences = [NSDictionary dictionaryWithObject:@"192.168.10.19" forKey:@"remoteHostName"];
    [[[NSUserDefaultsController sharedUserDefaultsController] defaults] registerDefaults:defaultPreferences];
}

- (void)didEndConnectionFailedSheet:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [alert.window orderOut:self];
    [self beginConnectionSheet];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    [self beginConnectionSheet];
}

- (void)beginConnectionSheet
{
    // Present modal "open connection" sheet.
    [NSApp beginSheet:connectWindow
       modalForWindow:window
        modalDelegate:self
       didEndSelector:@selector(didEndConnectSheet:returnCode:contextInfo:)
          contextInfo:NULL];
}

- (IBAction)openConnection: (id)sender
{
    [NSApp endSheet:connectWindow returnCode:1];
}

- (IBAction)cancelConnection: (id)sender
{
    [NSApp endSheet:connectWindow returnCode:0];
}

- (void)didEndConnectSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    // Dismiss the sheet.
    [sheet orderOut:self];
    
    if (returnCode == 1)
    {
        // Look up the host name that the user picked.
        NSString* remoteHostName = [[[NSUserDefaultsController sharedUserDefaultsController] defaults] stringForKey:@"remoteHostName"];
        // Create a proxy object for the host picked by the user.
        NSString* prxString = [NSString stringWithFormat:@"osc:tcp -h %@ -p 10000 -t 1", remoteHostName];
        
        // Tell the DAQ about us, or present a new 'open connection' sheet
        // if that fails.
        @try {
            ICEObjectPrx* prx = [communicator stringToProxy:prxString];
            oscPrx = [ramsonarscopeOscilloscopePrx uncheckedCast:prx];
            [oscPrx SetViewer:selfPrx];
        } @catch (NSException* ex) {
            NSAlert* alert = [[[NSAlert alloc] init] autorelease];
            [alert addButtonWithTitle:@"OK"];
            alert.messageText = [NSString stringWithFormat:@"Cannot connect to host \"%@\". Make sure that the remote DAQ program is running and is reachable on the network, and try again.", remoteHostName];
            alert.informativeText = ex.reason;
            alert.alertStyle = NSWarningAlertStyle;
            [alert beginSheetModalForWindow:window modalDelegate:self didEndSelector:@selector(didEndConnectionFailedSheet:returnCode:contextInfo:) contextInfo:NULL];
            return;
        }
        
        // Send default settings to DAQ, or crash if that fails.
        @try {
            [oscPrx SetScopeMode: ramsonarscopeScopeModeOscilloscope];
            [oscPrx SetTriggerMode: ramsonarscopeTriggerModeStop];
            [oscPrx SetTriggerSlope: ramsonarscopeTriggerSlopeRising];
            [oscPrx SetTriggerLevel: 0];
            [oscPrx SetHorizontalZoom: 0];
            
            [oscPrx retain];
        } @catch (NSException* ex) {
            [self crashWithException:ex];
        }
    } else {
        [window close];
    }
}

- (void)NotifyCapture:(ICECurrent*)current {
    ramsonarscopeOscilloscopeCapture* capture;
    @try {
        capture = [oscPrx GetLastCapture];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
        return;
    }
    [triggerModeControl setSelectedSegment: [capture newTriggerMode]];
    view.lastCapture = capture;
    [view setNeedsDisplay: YES];
    [triggerProgressIndicator stopAnimation: self];
}

- (IBAction)scopeModeChanged: (id)sender
{
    ramsonarscopeScopeMode scopeMode = [sender selectedSegment];
    @try {
        [oscPrx SetScopeMode: scopeMode];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
        return;
    }
    view->scopeMode = scopeMode;
    
    if (scopeMode == ramsonarscopeScopeModeSpectrumAnalyzer)
        [self updateSpectrum];
    
    [view setNeedsDisplay: YES];
}

- (IBAction)triggerLevelChanged: (id)sender
{
    short val = [sender floatValue]/10 * 300 * (1 << view->verticalSensitivity[view->triggerChannel]);
    @try {
        [oscPrx SetTriggerLevel: val];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
        return;
    }
    view->triggerLevel = val;
    [view setNeedsDisplay: YES];
}

- (IBAction)triggerChannelChanged: (id)sender
{
    short val = [sender selectedSegment];
    @try {
        [oscPrx SetTriggerChannel: val];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
    }
    view->triggerChannel = val;
    [view setNeedsDisplay: YES];
}

- (IBAction)triggerModeChanged: (id)sender
{
    if ([sender selectedSegment] == ramsonarscopeTriggerModeRun)
        [triggerProgressIndicator startAnimation: self];
    
    ramsonarscopeTriggerMode val = [sender selectedSegment];
    @try {
        [oscPrx SetTriggerMode: val];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
        return;
    }
}

- (IBAction)triggerSlopeChanged: (id)sender
{
    ramsonarscopeTriggerSlope val = [sender selectedSegment];
    @try {
        [oscPrx SetTriggerSlope: val];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
        return;
    }
}

- (IBAction)horizontalZoomChanged: (id)sender
{
    float val = [sender floatValue];
    @try {
        [oscPrx SetHorizontalZoom: val];
    } @catch (ICEException* ex) {
        [self crashWithException:ex];
        return;
    }
}

- (void)windowWillClose:(NSNotification *)aNotification {
    [NSApp terminate: self];
}

- (void)updateSpectrum
{
    if (view->scopeMode == ramsonarscopeScopeModeSpectrumAnalyzer)
    {
        @try {
            view.lastSpectrum = [oscPrx GetLastSpectrum];
        } @catch (ICETimeoutException *ex) {
            // If a timeout occurs, do nothing.
        } @catch (NSException *ex) {
            // If anything else goes wrong, show an error message and exit.
            [self crashWithException:ex];
            return;
        }
        [view setNeedsDisplay: YES];
        [self performSelector: @selector(updateSpectrum)
                   withObject: nil
                   afterDelay: 1/20.0];
    }
}

@end
