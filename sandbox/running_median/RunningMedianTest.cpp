/*
 * RunningMedianTest.cpp
 *
 *  Created on: Nov 4, 2008
 *      Author: david
 */

#include "RunningMedianTree.h"

#include <iostream>

int main (char** argv, unsigned int argc) {
	RunningMedianTree<double> test = RunningMedianTree<double>(32);
	for (int i = -32; i < 16; ++i) {
		test.push (i);
		std::cout << "inserted: " << i << ", median: " << test.median() << "\n";
	}
	return 0;
}
