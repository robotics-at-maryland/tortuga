// Copyright 2004-2007 Roman Yakovenko.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __constructors_bug_to_be_exported_hpp__
#define __constructors_bug_to_be_exported_hpp__

struct Joint{};
struct Actor{};

class JointCallback {    
public:

    class JointBreakCallback {
        public:
            virtual void onBreak(Joint*, float, Actor*, Actor*) {}
    };

    void onBreak(Joint* j, float impulse, Actor* a, Actor* b) {
        mCallback->onBreak(j,impulse, a,b);
    }

    JointBreakCallback*	mCallback;
};


#endif//__constructors_bug_to_be_exported_hpp__
