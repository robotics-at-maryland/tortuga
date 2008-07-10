//Michael Levashov
//06/29/08
//Created for Robotics at Maryland
//Copyright 2008
//
//A constant-size circular array class
//It sacrifices some space in order to make the reads of the array as fast as possible
//Note that it is not an intelligent array and data gets moved back as new one is added.  It is up to the master program to keep track of how much data it wrote and how much the old data got pushed back
//The class doesn't do any array boundary checking, so the master program needs to take care of that.  I could put it in, but it would make it slower

#ifndef _CIRC_ARRAY_
#define _CIRC_ARRAY_

template<class Type,  int N>
class circArray
{
    Type array[2*N];
    int index; //index represents the location of the most recently added element, goes from 0 to N-1

    public:
    //Refills the array with new data
    void refill(Type* buffer)
    {
        for(int k=0; k<N; k++)
        {
            array[k]=buffer[k];
            array[k+N]=buffer[k];
        }
        index=N;
    }

    circArray()
    {
        bzero(array,2*N*sizeof(int));

        index=N;
    }

    //copies from an existing array, buffer[0] is assumed to be the newest element
    circArray(Type* buffer)
    {
        refill(buffer);
    }

    ~circArray() {}

    //Adds a new element to the array, overwriting the older ones 
    void update(Type value)
    {
        index--;
        if(index<0)
            index+=N;

        array[index]=value;
        array[index+N]=value;
    }

    //Returns an element of the array.  The element at 0 is the newest element
    Type operator[](int n)
    {
        return array[n+index];
    }

    //NOTE: the use of the star is opposite to the normal behavior! It is not a dereference operator as usual, but returns a pointer
    Type* operator*()
    {
        return &array[index];
    }

    int length()
    {
        return N;
    }
};

#endif
