//
//  OscilloscopeViewControl.m
//  BlackfinScope
//
//  Created by Leo Singer on 7/27/09.
//  Copyright 2009 Robotics@Maryland. All rights reserved.
//

#import "OscilloscopeView.h"
#include <math.h>


@implementation OscilloscopeView
@synthesize lastCapture, lastSpectrum;

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        graticulePath = nil;
        axesPath = nil;
        lastCapture = nil;
        lastSpectrum = nil;
        scopeMode = ramsonarscopeScopeModeOscilloscope;
        
        triggerLevel = 0;
        triggerChannel = 0;
        
        channelColors[0] = [NSColor cyanColor];
        channelColors[1] = [NSColor magentaColor];
        channelColors[2] = [NSColor orangeColor];
        channelColors[3] = [NSColor blackColor];
        
        for (int i = 0 ; i < 4 ; i ++)
        {
            verticalSensitivity[i] = 0;
            verticalPosition[i] = 0;
            visibleChannels[i] = YES;
        }
    }
    return self;
}

- (IBAction)verticalSensitivityChanged:(id)sender
{
    int val = [sender floatValue];
    
    if ([verticalSensitivityCoupled state] == NSOnState)
        for (int channel = 0 ; channel < 4 ; channel ++)
            verticalSensitivity[channel] = val;
    else
        verticalSensitivity[[sender tag]] = val;
    
    [self setNeedsDisplay:YES];
}

- (IBAction)verticalPositionChanged:(id)sender
{
    int val = [sender floatValue]/10 * 300 * (1 << verticalSensitivity[[sender tag]]);
    
    if ([verticalPositionCoupled state] == NSOnState)
        for (int channel = 0 ; channel < 4 ; channel ++)
            verticalPosition[channel] = val;
    else
        verticalPosition[[sender tag]] = val;
    
    [self setNeedsDisplay:YES];
}

- (IBAction)visibleChannelsChanged:(id)sender
{
    for (int i = 0 ; i < 4 ; i ++)
        visibleChannels[i] = [sender isSelectedForSegment:i];
    [self setNeedsDisplay:YES];
}

- (IBAction)channelColorsChanged:(id)sender
{
    channelColors[[sender tag]] = [sender color];
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect {
    
    int width = [self frame].size.width;
    int height = [self frame].size.height;
    
    NSRect borderRect = NSMakeRect(0, 0, width, height);
    
    [[NSColor whiteColor] set];
    NSRectFill(borderRect);
    
    [[NSColor blackColor] setStroke];
    
    if (scopeMode == ramsonarscopeScopeModeOscilloscope)
    {
        // Draw the graticule
        if (graticulePath == nil)
        {
            graticulePath = [[NSBezierPath alloc] init];
            
            for (int i = 1 ; i < 10 ; i ++)
            {
                [graticulePath moveToPoint:NSMakePoint(width * i / 10, 0)];
                [graticulePath lineToPoint:NSMakePoint(width * i / 10, height)];
                [graticulePath moveToPoint:NSMakePoint(0, height * i / 10)];
                [graticulePath lineToPoint:NSMakePoint(width, height * i / 10)];
            }
            
            [graticulePath setLineWidth: 0.1];
        }
        [graticulePath stroke];
        
        
        // Draw the axes
        if (axesPath == nil)
        {
            axesPath = [[NSBezierPath alloc] init];
            
            [axesPath moveToPoint:NSMakePoint(0, height / 2)];
            [axesPath lineToPoint:NSMakePoint(width, height / 2)];
            [axesPath moveToPoint:NSMakePoint(width / 2, 0)];
            [axesPath lineToPoint:NSMakePoint(width / 2, height)];
            
            [axesPath setLineWidth: 2.0];
        }
        [axesPath stroke];
        
        if (lastCapture != nil)
        {
            const short * const data = [lastCapture.rawData bytes];
            for (int channel = 0 ; channel < 4 ; channel ++)
            {
                if (visibleChannels[channel])
                {
                    NSBezierPath* wave = [NSBezierPath bezierPath];
                    [wave moveToPoint:NSMakePoint(0, data[channel])];
                    
                    for (int i = 1 ; i < 800 ; i ++)
                        [wave lineToPoint:NSMakePoint(i, data[i * 4 + channel])];
                    
                    [wave setLineWidth:1.0];
                    
                    NSAffineTransform* transform = [NSAffineTransform transform];
                    [transform translateXBy:0 yBy:height/2];
                    [transform scaleXBy:1.0 yBy:-1.0/(1 << verticalSensitivity[channel])];
                    [transform translateXBy:0 yBy:verticalPosition[channel]];
                    [wave transformUsingAffineTransform:transform];
                    
                    [channelColors[channel] setStroke];
                    [wave stroke];
                }
            }
        }
        
        // Draw trigger line
        {
            NSBezierPath* triggerPath = [NSBezierPath bezierPath];
            [triggerPath moveToPoint:NSMakePoint(0, triggerLevel)];
            [triggerPath lineToPoint:NSMakePoint(width, triggerLevel)];
            
            NSAffineTransform* transform = [NSAffineTransform transform];
            [transform translateXBy:0 yBy:height/2];
            [transform scaleXBy:1.0 yBy:-1.0/(1 << verticalSensitivity[triggerChannel])];
            [transform translateXBy:0 yBy:verticalPosition[triggerChannel]];
            
            [triggerPath transformUsingAffineTransform:transform];
            
            [[NSColor grayColor] setStroke];
            [triggerPath stroke];
        }
        
        // Draw the border
        NSFrameRect(borderRect);
    } else /* scopeMode == ramsonarscopeScopeModeSpectrumAnalyzer */ {
        
        if (lastSpectrum != nil)
        {
            int nChannelVisible = 0;
            for (int i = 0 ; i < 4 ; i ++)
                nChannelVisible += visibleChannels[i];
            
            static const unsigned int maxHarmonic = 64;
            
            CGFloat xScale = width/(CGFloat)maxHarmonic;
            
            CGFloat curLevelBarWidth = (CGFloat)1/(nChannelVisible + 2);
            
            const uint32_t * const currentLevels = [[lastSpectrum currentLevels] bytes];
            const uint32_t * const peakLevels = [[lastSpectrum peakLevels] bytes];
            
            for (int k = 0 ; k < maxHarmonic ; k ++)
            {
                int iChannelVisible = 0;
                CGFloat highestBarHeight = 0;
                BOOL needsFreqLabel = NO;
                for (int channel = 0 ; channel < 4 ; channel ++)
                {
                    if (visibleChannels[channel])
                    {
                        uint32_t currentLevel = currentLevels[k * 4 + channel];
                        const uint32_t * const peakLevelPtr = &peakLevels[k * 4 + channel];
                        uint32_t peakLevel = *peakLevelPtr;
                        
                        if (
                            (k < 1 || peakLevel >= *(peakLevelPtr-1*4)) &&
                            (k < 2 || peakLevel >= *(peakLevelPtr-2*4)) &&
                            (k > 1 || *(peakLevelPtr-1*4) >= *(peakLevelPtr-2*4)) &&
                            (peakLevel >= *(peakLevelPtr+1*4)) &&
                            (peakLevel >= *(peakLevelPtr+2*4)) &&
                            (*(peakLevelPtr+1*4) >= *(peakLevelPtr+2*4))
                            )
                        {
                            needsFreqLabel = YES;
                        }
                        
                        CGFloat curLevelBarHeight = logf(currentLevel) / logf(1 << 16) * 600;
                        CGFloat peakLevelBarHeight = logf(peakLevel) / logf(1 << 16) * 600;
                        
                        if (peakLevelBarHeight > highestBarHeight)
                            highestBarHeight = peakLevelBarHeight;
                        
                        [channelColors[channel] set];
                        NSRectFill(NSMakeRect(
                            xScale*((iChannelVisible + 1)*curLevelBarWidth + k), 0,
                                              xScale*curLevelBarWidth, curLevelBarHeight));
                        
                        NSRectFill(NSMakeRect(xScale*k, peakLevelBarHeight, xScale*1, 2));
                        
                        iChannelVisible++;
                    }
                }
                
                if (needsFreqLabel)
                {
                    CGFloat freq = k * 500.0/512;
                    NSString* freqLabel = [NSString stringWithFormat: @"%0.1f kHz", freq];
                    
                    NSDictionary* freqLabelAttribs
                        = [NSDictionary dictionaryWithObject: [NSFont systemFontOfSize: 18]
                                                      forKey: NSFontAttributeName];
                    
                    [freqLabel drawAtPoint: NSMakePoint(xScale*(k - 0.2), highestBarHeight + 10)
                            withAttributes: freqLabelAttribs];
                }
            }
        }
        
    }
}

- (IBAction)presetsTile:(id)sender
{
    for (int channel = 0 ; channel < 4 ; channel ++)
    {
        verticalPosition[channel] = (channel * 0.5 - 0.75) * (double)300 * (1 << verticalSensitivity[channel]);
    }
    
    [self setNeedsDisplay:YES];
}


@end
