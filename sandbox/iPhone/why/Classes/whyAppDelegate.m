//
//  whyAppDelegate.m
//  why
//
//  Created by Local Admin on 3/20/09.
//  Copyright Robotics @ Maryland 2009. All rights reserved.
//

#import "whyAppDelegate.h"
#import "whyViewController.h"

@implementation whyAppDelegate

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
