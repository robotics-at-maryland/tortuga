//
//  poloAppDelegate.m
//  polo
//
//  Created by auvsi on 4/2/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "poloAppDelegate.h"
#import "poloViewController.h"

@implementation poloAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
