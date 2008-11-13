/*
 * DoubleHeapTest.h
 *
 *  Created on: Nov 8, 2008
 *      Author: david
 */

#ifndef DOUBLEHEAPTEST_H_
#define DOUBLEHEAPTEST_H_

#include <ostream>

#include "DoubleHeap.h"

template<class T>
class DoubleHeapTest: public DoubleHeap<T> {
public:
	DoubleHeapTest(unsigned int sampleSize): DoubleHeap<T>(sampleSize) {}

	void printInsertion (std::ostream& out) {
		for (int i = 0; i < this->max; ++i) {
			out << this->insertionOrder[i] << " ";
		}
		out << "current index: " << this->insertionOrderIndex << "\n";
	}

	void printHeap (std::ostream& out) {
		for (int i = this->lowerK + 1; i < this->upperK; ++i) {
			out << "index: " << i << " order: " << this->heap[i].order << " data: " << this->heap[i].data << "\n";
		}
	}
};

#endif /* DOUBLEHEAPTEST_H_ */
