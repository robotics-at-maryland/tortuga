//
//  OscilloscopeViewControl.m
//  BlackfinScope
//
//  Created by Leo Singer on 7/27/09.
//  Copyright 2009 Robotics@Maryland. All rights reserved.
//

#import "OscilloscopeView.h"


@implementation OscilloscopeView
@synthesize lastCapture;

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        graticulePath = nil;
        axesPath = nil;
        lastCapture = nil;
        
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
        short* data = [lastCapture.rawData bytes];
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
