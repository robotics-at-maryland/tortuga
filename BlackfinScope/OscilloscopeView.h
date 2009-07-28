//
//  OscilloscopeViewControl.h
//  BlackfinScope
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/27/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "scope.h"

@interface OscilloscopeView : NSView {

    NSBezierPath* graticulePath;
    NSBezierPath* axesPath;
    ramsonarscopeOscilloscopeCapture* lastCapture;
    
    int verticalSensitivity[4];
    
}
@property(retain) ramsonarscopeOscilloscopeCapture* lastCapture;
- (IBAction)verticalSensitivityChanged:(id)sender;
@end
