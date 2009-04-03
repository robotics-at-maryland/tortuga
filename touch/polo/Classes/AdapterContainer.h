//
//  AdapterContainer.h
//  polo
//
//  Created by auvsi on 4/2/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Ice/Ice.h>

@interface AdapterContainer : NSObject {
}
+(id<ICEObjectAdapter>) sharedAdapter;
+(void) setSharedAdapter: (id<ICEObjectAdapter>)adapter;
@end
