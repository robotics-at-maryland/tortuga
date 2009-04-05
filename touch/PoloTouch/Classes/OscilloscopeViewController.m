//
//  OscilloscopeViewController.m
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "OscilloscopeViewController.h"
#import <Ice/Ice.h>
#import "SharedIce.h"


@implementation OscilloscopeViewController

// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibNameAndHost:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil host:(NSURL *)host {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
        mURL = host;
    }
    return self;
}

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
    
    @try {
        {
            NSString* prxString = [NSString stringWithFormat:@"Marco:tcp -h %@ -p 10000", [mURL absoluteString]];
            id<ICEObjectPrx> prx = [[SharedIce sharedCommunicator] stringToProxy:prxString];
            mMarcoPrx = [rammarcopoloMarcoPrx checkedCast:prx];
            [mMarcoPrx retain];
        }
        
        {
            id<ICEObjectPrx> prx = [[SharedIce sharedAdapter] addWithUUID:mPoloI];
            id<rammarcopoloPoloPrx> poloPrx = [rammarcopoloPoloPrx checkedCast:prx];
            [mMarcoPrx registerPolo: poloPrx];
        }
    } @catch (NSException* ex) {
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Connection failed" message:[ex reason] delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
        [alert show];
    }
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    [self disconnect:self];
}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [super dealloc];
}


- (IBAction)disconnect: (id)sender {
    [self.navigationController popViewControllerAnimated: YES];
}

- (IBAction)toggleTriggerToolbar:(id)sender
{
    static bool toolbarVisible = false;
    if (toolbarVisible)
    {
        [mTriggerToolbar removeFromSuperview];
        toolbarVisible = false;
    } else {
        [self.view addSubview:mTriggerToolbar];
        toolbarVisible = true;
    }
}


-(void) reportData:(rammarcopoloSnapshot*)snap
{
    [mOscilloscopeView setSnapshot:snap];
}

-(void) triggerChanged:(short)newLevel
{
    [mOscilloscopeView triggerChanged:newLevel];
}

- (IBAction)setTriggerEdge:(id)sender {
    BOOL edge = [sender selectedSegmentIndex] == 0;
    [mMarcoPrx setTriggerEdge:edge];
}

- (IBAction)adjustTriggerLevel:(id)sender
{
    if ([sender selectedSegmentIndex] == 0)
        [mMarcoPrx decrementTriggerLevel:10];
    else
        [mMarcoPrx incrementTriggerLevel:10];
}

- (IBAction)setTriggerChannel:(id)sender
{
    [mMarcoPrx setTriggerChannel:[sender selectedSegmentIndex]];
}

@end
