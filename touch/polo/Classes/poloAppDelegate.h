//
//  poloAppDelegate.h
//  polo
//
//  Created by auvsi on 4/2/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class poloViewController;

@interface poloAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    poloViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet poloViewController *viewController;

@end

