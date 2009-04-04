//
//  PoloTouchAppDelegate.h
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright TRX Systems 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PoloTouchAppDelegate : NSObject <UIApplicationDelegate> {
    
    UIWindow *window;
    UINavigationController *navigationController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;

@end

