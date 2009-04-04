//
//  OscilloscopeViewController.h
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <marcopolo.h>
#import "PoloI.h"
#import "OscilloscopeView.h"
#import "GridView.h"


@interface OscilloscopeViewController : UIViewController {

    IBOutlet OscilloscopeView* mOscilloscopeView;
    IBOutlet GridView* mGridView;
    
    IBOutlet PoloI* mPoloI;
    id<rammarcopoloMarcoPrx> mMarcoPrx;
    NSURL* mURL;
    
}
-(void) reportData:(rammarcopoloSnapshot*)snap;
- (IBAction)disconnect: (id)sender;
- (id)initWithNibNameAndHost:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil host:(NSURL *)host;
@end
