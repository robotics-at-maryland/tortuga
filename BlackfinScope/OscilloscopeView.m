//
//  OscilloscopeViewControl.m
//  BlackfinScope
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/27/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "OscilloscopeView.h"


@implementation OscilloscopeView

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        graticulePath = nil;
        axesPath = nil;
    }
    return self;
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
    
    
    // Draw the border
    NSRect borderRect = NSMakeRect(0, 0, width, height);
    NSFrameRect(borderRect);
}



@end
