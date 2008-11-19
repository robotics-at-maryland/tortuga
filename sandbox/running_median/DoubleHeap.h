/**
 * @file DoubleHeap.h
 *
 * Created on: Nov 8, 2008
 * @author David Love
 */

#ifndef DOUBLEHEAP_H_
#define DOUBLEHEAP_H_

#include "CircularArray.h"

#include <cassert>

#define FATHER(i) ((2 * (i)) - 1)
#define MOTHER(i) ((2 * (i)) - 2)
#define CHILD(i) (- (( - (i) - 1) >> 1))

#define SON(i) ((2 * (i)) + 1)
#define DAUGHTER(i) ((2 * (i)) + 2)
#define PARENT(i) (((i) - 1) >> 1)

#define LARGER(i,j) (this->m_heap[(i)].m_data > this->m_heap[(j)].m_data ? (i) : (j))
#define SMALLER(i,j) (this->m_heap[(i)].m_data < this->m_heap[(j)].m_data ? (i) : (j))

#define NEXTT1(i) (exists(DAUGHTER(i)) ? LARGER(DAUGHTER(i), SON(i)) : SON(i))
#define NEXTT2(i) (exists(MOTHER(i)) ? SMALLER(MOTHER(i), FATHER(i)) : FATHER(i))

template<class T>
class DoubleHeap {
protected:
	// the node type
	struct Node {
		T m_data;
		int m_order;
	};
	const unsigned int m_max;
	unsigned int m_size;
	int m_upperK;
	int m_lowerK;
	int m_insertionOrderIndex;
	int* m_insertionOrder;
	CircularArray<Node> m_heap;
	inline bool exists(int i) const {
        return (i < m_upperK && i > m_lowerK);
	}
	inline void swap(int& i, int& j) {
		Node tempN = m_heap[i];
		m_heap[i] = m_heap[j];
		m_heap[j] = tempN;
		int tempI = m_insertionOrder[m_heap[i].m_order];
		m_insertionOrder[m_heap[i].m_order] = m_insertionOrder[m_heap[j].m_order];
		m_insertionOrder[m_heap[j].m_order] = tempI;
		i = j;
	}
	inline void balance(int i) {
		// move T2 nodes down to root
		int j = CHILD(i);
		while (i < 0 && exists(j) && m_heap[i].m_data < m_heap[j].m_data) {
			swap(i, j);
			j = CHILD(i);
		}
		// move T1 nodes up to root
		j = PARENT(i);
		while (i > 0 && exists(j) && m_heap[i].m_data > m_heap[j].m_data) {
			swap(i, j);
			j = PARENT(i);
		}
		// move T2 nodes up to the leaves
		j = NEXTT2(i);
		while (i <= 0 && exists(j) && m_heap[i].m_data > m_heap[j].m_data) {
			swap(i, j);
			j = NEXTT2(i);
		}
		// move T1 nodes down to the leaves
		j = NEXTT1(i);
		while (i >= 0 && exists(j) && m_heap[i].m_data < m_heap[j].m_data) {
			swap(i, j);
			j = NEXTT1(i);
		}
	}
public:
	DoubleHeap(unsigned int window) :
		m_max(window), m_size(0), m_upperK(1), m_lowerK(-1), m_insertionOrderIndex(0),
				m_insertionOrder(new int[window]), m_heap(CircularArray<Node> (
						window)) {
        assert ((window - 1) % 4 == 0);
		for (int i = 1; i <= m_max; ++i)
			m_insertionOrder[i - 1] = (i % 2 ? -1 : 1) * (i / 2);
	}
	~DoubleHeap() {
		delete m_insertionOrder;
	}
	inline const T& push(const T& data) {
		int i = m_insertionOrder[m_insertionOrderIndex];
		if (m_size < m_max) {
			if (i > 0)
				m_upperK = i + 1;
			else
				m_lowerK = i - 1;
			++m_size;
		}
		const T& v = m_heap[i].m_data;
		m_heap[i].m_data = data;
		m_heap[i].m_order = m_insertionOrderIndex++;
		if (m_insertionOrderIndex == m_max)
			m_insertionOrderIndex = 0;
		balance(i);
		return v;
	}
	inline const T& median() {
		return m_heap[0].m_data;
	}
    inline const unsigned int size() const {
        return m_size;
    }
};

#endif /* DOUBLEHEAP_H_ */
