//
//  PoloTouchAppDelegate.m
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright TRX Systems 2009. All rights reserved.
//

#import "PoloTouchAppDelegate.h"
#import "RootViewController.h"


@implementation PoloTouchAppDelegate

@synthesize window;
@synthesize navigationController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {
	
	// Configure and show the window
	[window addSubview:[navigationController view]];
	[window makeKeyAndVisible];
}


- (void)applicationWillTerminate:(UIApplication *)application {
	// Save data if appropriate
}


- (void)dealloc {
	[navigationController release];
	[window release];
	[super dealloc];
}

@end
