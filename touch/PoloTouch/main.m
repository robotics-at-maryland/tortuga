//
//  main.m
//  PoloTouch
//
//  Created by Lady 3Jane Marie-France Tessier-Ashpool on 4/3/09.
//  Copyright TRX Systems 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Ice/Ice.h>
#import "SharedIce.h"

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int status = 1;
    id<ICECommunicator> communicator = nil;
    @try {
        communicator = [ICEUtil createCommunicator:&argc argv:argv];
        
        id<ICEObjectAdapter> adapter =
        [communicator createObjectAdapterWithEndpoints: @"PoloAdapter"
                                             endpoints: @"default -p 10001"];
        [adapter activate];
        
        [SharedIce setSharedCommunicator:communicator];
        [SharedIce setSharedAdapter:adapter];
        
        status = UIApplicationMain(argc, argv, nil, nil);
        
        [communicator waitForShutdown];
    } @catch (NSException* ex) {
        NSLog(@"%@", ex);
    }
    
    @try {
        [communicator destroy];
    } @catch (NSException* ex) {
        NSLog(@"%@", ex);
    }
    
    [pool release];
    return status;
}
