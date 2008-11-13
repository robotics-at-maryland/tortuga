/*
 * CircularArray.h
 *
 *  Created on: Nov 8, 2008
 *      Author: David Love
 *
 *      A simple extension of the C++ array allowing negative indexing.
 */

#ifndef CIRCULARARRAY_H_
#define CIRCULARARRAY_H_

template<class T>
class CircularArray {
protected:
	const unsigned int max;
	T* data;
public:
	CircularArray(const unsigned int size) :
		max(size), data(new T[size]) {
	}
	~CircularArray () {
		delete data;
	}
	T& operator[] (int index) {
		if (index < 0) {
			index = max + index;
		}
		return data[index];
	}
};

#endif /* CIRCULARARRAY_H_ */
