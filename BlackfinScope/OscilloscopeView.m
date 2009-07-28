//
//  OscilloscopeViewControl.m
//  BlackfinScope
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/27/09.
//  Copyright 2009 TRX Systems. All rights reserved.
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
        
        for (int i = 0 ; i < 4 ; i ++)
            verticalSensitivity[i] = 0;
    }
    return self;
}

- (IBAction)verticalSensitivityChanged:(id)sender
{
    verticalSensitivity[[sender tag]] = [sender floatValue];
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect {
    
    int width = [self frame].size.width;
    int height = [self frame].size.height;
    
    
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
            NSBezierPath* wave = [NSBezierPath bezierPath];
            [wave moveToPoint:NSMakePoint(0, data[channel])];
            
            for (int i = 1 ; i < 800 ; i ++)
                [wave lineToPoint:NSMakePoint(i, data[i * 4 + channel])];
            
            [wave setLineWidth:2.0];
            
            NSAffineTransform* transform = [NSAffineTransform transform];
            [transform translateXBy:0 yBy:height/2];
            [transform scaleXBy:1.0 yBy:1.0/(1 << verticalSensitivity[channel])];
            [wave transformUsingAffineTransform:transform];
            
            [wave stroke];
        }
    }
    
    // Draw the border
    NSRect borderRect = NSMakeRect(0, 0, width, height);
    NSFrameRect(borderRect);
}



@end
