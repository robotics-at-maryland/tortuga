//
//  PoloI.m
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/4/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "PoloI.h"


@implementation PoloI
-(void) reportData:(rammarcopoloSnapshot *)snap current:(ICECurrent *)current
{
    [mOscilloscopeViewController reportData:snap];
}
-(void) triggerChanged:(short)newLevel current:(ICECurrent *)current
{
    [mOscilloscopeViewController triggerChanged:newLevel];
}
@end
