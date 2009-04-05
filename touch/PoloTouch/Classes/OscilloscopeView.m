//
//  OscilloscopeView.m
//  polo
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "OscilloscopeView.h"
#import <CoreGraphics/CoreGraphics.h>
#include <math.h>

@implementation OscilloscopeView


- (void)setSnapshot:(rammarcopoloSnapshot*)snapshot
{
    @synchronized(self)
    {
        if (mSnapshot != nil)
            [mSnapshot release];
        mSnapshot = [snapshot retain];
    }
    [self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];
}


- (void)triggerChanged:(short)newLevel
{
    /*
    @synchronized(self)
    {
        triggerLevel = newLevel;
    }
    [self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];
     */
}


- (rammarcopoloSnapshot*)snapshot
{
    return mSnapshot;
}



- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    rammarcopoloSnapshot* snapshot = nil;
    
    @synchronized(self)
    {
        if (mSnapshot != nil)
        {
            snapshot = [mSnapshot retain];
        }
    }
    
    if (snapshot != nil)
    {
        short* data = (short*)[[snapshot samples] bytes];
        triggerLevel = snapshot.triggerLevel;

        CGContextRef context = UIGraphicsGetCurrentContext();
        
        CGContextSaveGState(context);
        
        CGContextSetStrokeColorWithColor(context, CGColorGetConstantColor(kCGColorBlack));
        CGContextSetLineWidth(context, 2);
        
        CGContextTranslateCTM(context, 0, self.bounds.size.height/2);
        CGContextScaleCTM(context, 1.0f, 0.1f);
        
        CGContextBeginPath(context);
        CGContextMoveToPoint(context, 0, data[0]);
                
        int i;
        for (i = 1 ; i < 480 ; i ++)
        {
            CGContextAddLineToPoint(context, i, data[i]);
        }
        
        CGContextStrokePath(context);
        
        CGColorRef triggerLevelColor = CGColorCreateGenericGray(0.15f, 1.0f);
        CGContextSetStrokeColorWithColor(context, triggerLevelColor);
        CGColorRelease(triggerLevelColor);
        CGContextBeginPath(context);
        CGContextMoveToPoint(context, 0, triggerLevel);
        CGContextAddLineToPoint(context, self.bounds.size.width, triggerLevel);
        CGContextStrokePath(context);
        
        CGContextRestoreGState(context);
        
        [snapshot release];
    }
}


- (void)dealloc {
    [super dealloc];
}


@end
