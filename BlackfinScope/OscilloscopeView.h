//
//  OscilloscopeViewControl.h
//  BlackfinScope
//
//  Created by Leo Singer on 7/27/09.
//  Copyright 2009 Robotics@Maryland. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "scope.h"

@interface OscilloscopeView : NSView {

    IBOutlet NSButton* verticalSensitivityCoupled;
    IBOutlet NSButton* verticalPositionCoupled;
    NSBezierPath* graticulePath;
    NSBezierPath* axesPath;
    ramsonarscopeOscilloscopeCapture* lastCapture;
    ramsonarscopeSpectrumCapture* lastSpectrum;
    
    @public
    ramsonarscopeScopeMode scopeMode;
    short triggerLevel;
    short triggerChannel;
    int verticalSensitivity[4];
    int verticalPosition[4];
    bool visibleChannels[4];
    NSColor* channelColors[4];
    
}
@property(retain) ramsonarscopeOscilloscopeCapture* lastCapture;
@property(retain) ramsonarscopeSpectrumCapture* lastSpectrum;
- (IBAction)verticalSensitivityChanged:(id)sender;
- (IBAction)verticalPositionChanged: (id)sender;
- (IBAction)visibleChannelsChanged:(id)sender;
- (IBAction)channelColorsChanged:(id)sender;
- (IBAction)presetsTile:(id)sender;
@end
