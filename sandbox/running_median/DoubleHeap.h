/**
 * @file DoubleHeap.h
 *
 * Created on: Nov 8, 2008
 * @author David Love
 */

#ifndef DOUBLEHEAP_H_
#define DOUBLEHEAP_H_

#include "CircularArray.h"

#include <math.h>

#define FATHER(i) ((2 * (i)) - 1)
#define MOTHER(i) ((2 * (i)) - 2)
//#define CHILD(i) (0 - floor (((float) ((0 - (i)) - 1)) / ((float) 2)))
#define CHILD(i) (- (( - (i) - 1) >> 1))

#define SON(i) ((2 * (i)) + 1)
#define DAUGHTER(i) ((2 * (i)) + 2)
//#define PARENT(i) floor (((float) ((i) - 1)) / ((float) 2))
#define PARENT(i) (((i) - 1) >> 1)

#define LARGER(i,j) (heap[(i)].data > heap[(j)].data ? (i) : (j))
#define SMALLER(i,j) (heap[(i)].data < heap[(j)].data ? (i) : (j))

#define NEXTT1(i) (exists(DAUGHTER(i)) ? LARGER(DAUGHTER(i), SON(i)) : SON(i))
#define NEXTT2(i) (exists(MOTHER(i)) ? SMALLER(MOTHER(i), FATHER(i)) : FATHER(i))

template<class T>
class DoubleHeap {
protected:
	// the node type
	struct Node {
		T data;
		int order;
		Node() :
			order(-1) {
		}
	};
	const unsigned int max;
	unsigned int size;
	int upperK;
	int lowerK;
	int insertionOrderIndex;
	int* insertionOrder;
	CircularArray<Node> heap;
	inline bool exists(int i) {
        return (i < upperK && i > lowerK);
	}
	inline void swap(int& i, int& j) {
		Node tempN = heap[i];
		heap[i] = heap[j];
		heap[j] = tempN;
		int tempI = insertionOrder[heap[i].order];
		insertionOrder[heap[i].order] = insertionOrder[heap[j].order];
		insertionOrder[heap[j].order] = tempI;
		i = j;
	}
	inline void balance(int i) {
		// move T2 nodes down to root
		int j = CHILD(i);
		while (i < 0 && exists(j) && heap[i].data < heap[j].data) {
			swap(i, j);
			j = CHILD(i);
		}
		// move T1 nodes up to root
		j = PARENT(i);
		while (i > 0 && exists(j) && heap[i].data > heap[j].data) {
			swap(i, j);
			j = PARENT(i);
		}
		// move T2 nodes up to the leaves
		j = NEXTT2(i);
		while (i <= 0 && exists(j) && heap[i].data > heap[j].data) {
			swap(i, j);
			j = NEXTT2(i);
		}
		// move T1 nodes down to the leaves
		j = NEXTT1(i);
		while (i >= 0 && exists(j) && heap[i].data < heap[j].data) {
			swap(i, j);
			j = NEXTT1(i);
		}
	}
public:
	DoubleHeap(unsigned int window) :
		max(window), size(0), upperK(1), lowerK(-1), insertionOrderIndex(0),
				insertionOrder(new int[window]), heap(CircularArray<Node> (
						window)) {
		if ((window - 1) % 4) {
			throw "window size does not satisfy 4k + 1";
		}
		//insertionOrder[0] = 0;
		for (int i = 1; i <= max; ++i) {
			insertionOrder[i - 1] = (i % 2 ? -1 : 1) * (i / 2);
		}
	}
	~DoubleHeap() {
		delete insertionOrder;
	}
	inline const T& push(const T& data) {
		int i = insertionOrder[insertionOrderIndex];
		if (size < max) {
			if (i > 0)
				upperK = i + 1;
			else
				lowerK = i - 1;
			++size;
		}
		const T& v = heap[i].data;
		heap[i].data = data;
		heap[i].order = insertionOrderIndex++;
		if (insertionOrderIndex >= max)
			insertionOrderIndex = 0;
		balance(i);
		return v;
	}
	inline const T& median() {
		return heap[0].data;
	}
};

#endif /* DOUBLEHEAP_H_ */
