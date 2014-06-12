//
//  whyAppDelegate.h
//  why
//
//  Created by Local Admin on 3/20/09.
//  Copyright Robotics @ Maryland 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class whyViewController;

@interface whyAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    whyViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet whyViewController *viewController;

@end

