//
//  OscilloscopeController.h
//  BlackfinScope
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 7/27/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "scope.h"
#import "OscilloscopeView.h"


@interface OscilloscopeController : ramsonarscopeViewer <ramsonarscopeViewer> {

    id<ramsonarscopeOscilloscopePrx> oscPrx;
    OscilloscopeView* viewPrx;
    
}
- (IBAction)triggerChannelChanged: (id)sender;
- (IBAction)triggerModeChanged: (id)sender;
- (IBAction)triggerSlopeChanged: (id)sender;
@end
