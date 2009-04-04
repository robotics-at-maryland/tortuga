//
//  GridView.m
//  polo
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "GridView.h"
#import <CoreGraphics/CoreGraphics.h>


@implementation GridView


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextSaveGState(context);
    
    CGContextSetStrokeColorWithColor(context, CGColorGetConstantColor(kCGColorBlack));
    
    CGFloat dashes[] = {4,2};
    CGContextSetLineDash(context, 0, dashes, sizeof(dashes)/sizeof(CGFloat));
    CGContextSetLineWidth(context, 0.5);
    
    CGContextBeginPath(context);
    
    CGFloat width = [self bounds].size.width;
    CGFloat height = [self bounds].size.height;
    
    CGFloat xstep = width / 10;
    CGFloat ystep = height / 10;
    
    int i;
    for (i = 0 ; i < 10 ; i ++)
    {
        int lineHeight = i * ystep;
        CGContextMoveToPoint(context, 0, lineHeight);
        CGContextAddLineToPoint(context, width, lineHeight);
        
        int lineWidth = i * xstep;
        CGContextMoveToPoint(context, lineWidth, 0);
        CGContextAddLineToPoint(context, lineWidth, height);
    }
    
    CGContextStrokePath(context);
    
    CGContextRestoreGState(context);
}


- (void)dealloc {
    [super dealloc];
}


@end
