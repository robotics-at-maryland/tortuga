//
//  OscilloscopeView.h
//  polo
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <marcopolo.h>


@interface OscilloscopeView : UIView {
    rammarcopoloSnapshot* mSnapshot;
}
- (void)setSnapshot:(rammarcopoloSnapshot*)snapshot;
@end
