/*
 *  mockobserver.h
 *  MarsSuitOCI
 *
 *  Created by Joseph Lisee on 3/4/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __MOCKOBSERVER_H
#define __MOCKOBSERVER_H

// STL Includes
#include <vector>
#include <map>

// Project Includes
#include "foundation/pattern/h/observer.h"

namespace ssl {
namespace foundation {
namespace pattern {

// Mock class for testing
class MockObserver : public Observer 
{
public:    

    
    int callCount;
    void *data;
    long flag;
    std::map<long, int> flagCountMap;
    std::vector<void *> passedData;
    std::vector<long> passedFlags;

    
    MockObserver():
        callCount(0),
        data(0),
        flag(0),
        flagCountMap(),
        passedData(0),
        passedFlags(0)

    {
    }
    
    ~MockObserver()
    {
    }
    
    void update(Subject* o, void* data = 0, long flag = 0)
    {
        callCount++;
        this->data = data;
        this->flag = flag;
        
        passedData.push_back(data);
        passedFlags.push_back(flag);
        
        if(flagCountMap.find(flag) == flagCountMap.end())
            flagCountMap[flag] = 1;
        else
            flagCountMap[flag] = flagCountMap[flag] + 1;
    }
};

} // namespace pattern
} // namespace foundation
} // namespace ssl

#endif // __MOCKOBSERVER_H
