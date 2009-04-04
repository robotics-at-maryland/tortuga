//
//  SharedIce.m
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/4/09.
//  Copyright 2009 TRX Systems. All rights reserved.
//

#import "SharedIce.h"

id<ICECommunicator> mCommunicator = nil;
id<ICEObjectAdapter> mAdapter = nil;

@implementation SharedIce
+ (void)setSharedCommunicator: (id<ICECommunicator>)communicator
{
    mCommunicator = communicator;
}
+ (id<ICECommunicator>)sharedCommunicator
{
    return mCommunicator;
}
+ (void)setSharedAdapter: (id<ICEObjectAdapter>)adapter
{
    mAdapter = adapter;
}
+ (id<ICEObjectAdapter>)sharedAdapter
{
    return mAdapter;
}
@end
