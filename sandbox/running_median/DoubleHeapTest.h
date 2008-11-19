/**
 * DoubleHeapTest.h
 *
 *  Created on: Nov 8, 2008
 *      Author: David Love
 *      Robotics At Maryland
 */

#ifndef DOUBLEHEAPTEST_H_
#define DOUBLEHEAPTEST_H_

#include <ostream>
#include <vector>
#include <cassert>

#include "DoubleHeap.h"

/**
 * A class extending DoubleHeap<T> to add functions for testing.
 */
template<class T>
class DoubleHeapTest: public DoubleHeap<T>
{
protected:
    void getSamples(int point, std::vector<T>& data);
public:
    DoubleHeapTest(unsigned int size);
    void printInsertion(std::ostream& out);
    void printHeap(std::ostream& out);
    std::vector<T> samples();
    T& get(int index);
    void validate(int i);
    void push(T& data);
};

/**
 * Push data into the heap and then validate it.
 */

template<class T>
void DoubleHeapTest<T>::push(T& data)
{
    DoubleHeap<T>::push(data);
    if (this->m_size == this->m_max)
    {
        //validate(0);
    }
}

/**
 * Validate the heap.
 */
template<class T>
void DoubleHeapTest<T>::validate(int i)
{
    int mother = MOTHER(i);
    int father = FATHER(i);
    int son = SON(i);
    int daughter = DAUGHTER(i);
    if (i <= 0)
    {
        if (this->exists(mother))
        {
            assert (this->m_heap[mother].m_data >= this->m_heap[i].m_data);
            validate(mother);
        }
        if (this->exists(father))
        {
            assert (this->m_heap[father].m_data >= this->m_heap[i].m_data);
            validate(father);
        }
    }
    if (i >= 0)
    {
        if (this->exists(daughter))
        {
            assert (this->m_heap[daughter].m_data <= this->m_heap[i].m_data);
            validate(daughter);
        }
        if (this->exists(son))
        {
            assert (this->m_heap[son].m_data <= this->m_heap[i].m_data);
            validate(son);
        }
    }
}

/**
 * Build a double heap test object.
 */
template<class T>
DoubleHeapTest<T>::DoubleHeapTest(unsigned int size) :
    DoubleHeap<T> (size)
{
}
/**
 * Print the existing insertion order list and the current insertion order index.
 */
template<class T>
void DoubleHeapTest<T>::printInsertion(std::ostream& out)
{
    for (int i = 0; i < this->m_max; ++i)
    {
        out << this->m_insertionOrder[i] << " ";
    }
    out << "current index: " << this->m_insertionOrderIndex << "\n";
}
/**
 * Print out the current heap status including the location in the
 * insertion order list and data stored at each index in the list.
 */
template<class T>
void DoubleHeapTest<T>::printHeap(std::ostream& out)
{
    for (int i = this->m_lowerK; i <= this->m_upperK; ++i)
    {
        out << "index: " << i << " order: " << this->m_heap[i].m_order
                << " data: " << this->m_heap[i].m_data << "\n";
    }
}

/**
 * Recursively build a list of the samples in order (least to greatest)
 */
template<class T>
void DoubleHeapTest<T>::getSamples(int i, std::vector<T>& data)
{
    int mother = MOTHER(i);
    int father = FATHER(i);
    int son = SON(i);
    int daughter = DAUGHTER(i);
    if (i > 0)
    {
        // T1
        if (this->exists(daughter))
        {
            getSamples(SMALLER(daughter,son), data);
            getSamples(LARGER(daughter, son), data);
        }
        else if (this->exists(son))
        {
            getSamples(son, data);
        }
        data.push_back(this->m_heap[i].m_data);
    }
    else if (i < 0)
    {
        // T2
        data.push_back(this->m_heap[i].m_data);
        if (this->exists(mother))
        {
            getSamples(SMALLER(mother,father), data);
            getSamples(LARGER(mother, father), data);
        }
        else if (this->exists(father))
        {
            getSamples(father, data);
        }
    }
    else
    {
        // ROOT
        if (this->exists(daughter))
        {
            getSamples(SMALLER(daughter,son), data);
            getSamples(LARGER(daughter, son), data);
        }
        else if (this->exists(son))
        {
            getSamples(son, data);
        }
        data.push_back(this->m_heap[i].m_data);
        if (this->exists(mother))
        {
            getSamples(SMALLER(mother,father), data);
            getSamples(LARGER(mother, father), data);
        }
        else if (this->exists(father))
        {
            getSamples(father, data);
        }
    }
}

/**
 * Create and return a std::vector containing the data points in the heap in order.
 */
template<class T>
std::vector<T> DoubleHeapTest<T>::samples()
{
    std::vector<T> data = std::vector<T>();
    getSamples(0, data);
    return data;

}

/**
 * Get the value in the heap at the given index.
 */
template<class T>
T& DoubleHeapTest<T>::get(int index)
{
    return this->m_heap[index].m_data;
}

#endif /* DOUBLEHEAPTEST_H_ */
