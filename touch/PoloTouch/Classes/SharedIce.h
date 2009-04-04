//
//  SharedIce.h
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/4/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Ice/Ice.h>


@interface SharedIce : NSObject {
}
+ (void)setSharedCommunicator: (id<ICECommunicator>)communicator;
+ (id<ICECommunicator>)sharedCommunicator;
+ (void)setSharedAdapter: (id<ICEObjectAdapter>)adapter;
+ (id<ICEObjectAdapter>)sharedAdapter;
@end
