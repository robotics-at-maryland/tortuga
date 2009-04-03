//
//  AdapterContainer.m
//  polo
//
//  Created by auvsi on 4/2/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AdapterContainer.h"


static id<ICEObjectAdapter> mAdapter;

@implementation AdapterContainer
+(id<ICEObjectAdapter>) sharedAdapter
{
	return mAdapter;
}
+(void) setSharedAdapter: (id<ICEObjectAdapter>)adapter
{
	mAdapter = adapter;
}
@end
