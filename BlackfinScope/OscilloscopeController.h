//
//  OscilloscopeController.h
//  BlackfinScope
//
//  Created by Leo Singer on 7/27/09.
//  Copyright 2009 Robotics@Maryland. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "scope.h"
#import "OscilloscopeView.h"


@interface OscilloscopeController : ramsonarscopeViewer <ramsonarscopeViewer, NSApplicationDelegate> {

    id<ICECommunicator> communicator;
    id<ICEObjectAdapter> adapter;
    id<ramsonarscopeOscilloscopePrx> oscPrx;
    id<ramsonarscopeViewerPrx> selfPrx;
    
    IBOutlet NSSegmentedControl* triggerModeControl;
    IBOutlet NSProgressIndicator* triggerProgressIndicator;
    IBOutlet OscilloscopeView* view;
    IBOutlet NSWindow* window;
    IBOutlet NSWindow* connectWindow;
    
}

- (void)crashWithException:(NSException *)ex;
- (void)didEndCrashSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;

- (void)beginConnectionSheet;
- (void)didEndConnectionFailedSheet:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;
- (IBAction)openConnection: (id)sender;
- (IBAction)cancelConnection: (id)sender;
- (void)didEndConnectSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;

- (void) NotifyCapture:(ICECurrent *)current;
- (IBAction)scopeModeChanged: (id)sender;
- (IBAction)triggerLevelChanged: (id)sender;
- (IBAction)triggerChannelChanged: (id)sender;
- (IBAction)triggerModeChanged: (id)sender;
- (IBAction)triggerSlopeChanged: (id)sender;
- (IBAction)horizontalZoomChanged: (id)sender;

- (void) updateSpectrum;
@end
