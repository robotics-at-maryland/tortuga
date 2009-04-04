//
//  RootViewController.m
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright TRX Systems 2009. All rights reserved.
//

#import "RootViewController.h"
#import "PoloTouchAppDelegate.h"
#import "OscilloscopeViewController.h"


@implementation RootViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    self.navigationItem.hidesBackButton = NO;
}

/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
	[super viewDidDisappear:animated];
}
*/

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}



- (void)goForward
{
    // Navigation logic may go here. Create and push another view controller.
    OscilloscopeViewController* oscilloscopeViewController = [[OscilloscopeViewController alloc] initWithNibNameAndHost:@"OscilloscopeViewController" bundle: nil host: [NSURL URLWithString: [textFieldHost text]]];
    [self.navigationController pushViewController:oscilloscopeViewController animated:true];
    [oscilloscopeViewController release];
}



- (void)dealloc {
    [super dealloc];
}


@end

