/*
 * RunningMedianTest.cpp
 *
 *  Created on: Nov 4, 2008
 *      Author: david
 */

#include "DoubleHeap.h"
#include "DoubleHeapTest.h"

#include <iostream>

#define VALUE 5
#define heap DoubleHeapTest<double>

int main (char** argv, unsigned int argc) {
	heap test = heap(VALUE);
	//test.printInsertion(std::cout);
	double v;
	for (double i = 0; i <= VALUE * 2; ++i) {
		v = test.push (i);
		std::cout << "inserted: " << i << " replaced: " << v << " median: " << test.median() << "\n";
		test.printHeap(std::cout);
		test.printInsertion(std::cout);
		std::cout << std::endl;
	}
	return 0;
}
