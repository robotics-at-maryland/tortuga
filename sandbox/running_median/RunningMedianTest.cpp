/**
 * RunningMedianTest.cpp
 *
 *  Created on: Nov 4, 2008
 *      Author: David Love
 *      Robotics At Maryland
 */

//#include "DoubleHeap.h"
#include "DoubleHeapTest.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <sys/time.h>
#include <cassert>

/**
 * Build a divergent series of length n
 */
std::vector<int> divergentSeries(int n)
{
    std::vector<int> data = std::vector<int>();
    for (int i = 0; i < n; ++i)
    {
        data.push_back(((i % 2) ? -1 : 1) * i / 2);
    }
    return data;
}

/**
 * Build a random series of length n
 */
std::vector<int> randomSeries(int n)
{
    std::vector<int> data = std::vector<int>();
    srand(clock());
    for (int i = 0; i < n; ++i)
    {
        data.push_back(rand());
    }
    return data;
}

/**
 * Print the values in a vector.
 */
void printVector(std::vector<int>& data, std::ostream& out)
{
    for (int i = 0; i < data.size(); ++i)
    {
        out << data[i] << " ";
    }
    out << "\n";
}

/**
 * Test the accuracy of the double median filter.
 */
void accuracyTest(int samples, int window, std::ostream& out)
{
    std::vector<int> data = randomSeries(samples);
    DoubleHeapTest<int> heap = DoubleHeapTest<int> (window);
    std::vector<int> vector1, vector2;
    int median1, median2;
    int fail = 0;
    for (int i = 0; i < data.size(); ++i)
    {
        heap.push(data[i]);
        if (heap.size() >= window)
        {
            out << "Insertion " << i << "\n";
            //heap.validate (0);
            vector1 = vector2 = heap.samples();
            median1 = vector1[vector1.size() / 2];
            std::sort(vector2.begin(), vector2.end());
            median2 = vector2[vector2.size() / 2];
            if (heap.median() != median1 || heap.median() != median2 || median1
                    != median2)
            {
                out << "Fail: " << heap.median() << " != " << median1 << " != "
                        << median2 << " on insert " << i << "\n";
                out << "heap: ";
                printVector(vector1, out);
                out << "vector: ";
                printVector(vector2, out);
                ++fail;
            }
        }
    }
    fail = data.size() - fail;
    out << "Test complete. Passed: " << fail << "/" << data.size() << "\n";
}

/**
 * Time the insertion of a series of samples into a heap with
 * a given window size.
 */
float timedHeap(std::vector<int> data, int window)
{
    DoubleHeap<int> test = DoubleHeap<int> (window);
    timeval begin, end;
    gettimeofday(&begin, 0);
    for (unsigned int i = 0; i < data.size(); ++i)
    {
        test.push(data[i]);
    }
    gettimeofday(&end, 0);
    return ((float) difftime(end.tv_sec, begin.tv_sec))
            + (((float) (end.tv_usec - begin.tv_usec)) / 100000.0);
}

/**
 * Time the creation of a sorted std::vector with each set of
 * samples given a certain window size.
 */
float timedVector(std::vector<int> data, int window)
{
    std::vector<int> test = std::vector<int>();
    std::vector<int>::iterator found;
    timeval begin, end;
    gettimeofday(&begin, 0);
    for (int i = 0; i < data.size(); ++i)
    {
        test.push_back(data[i]);
        if (i >= window)
        {
            found = std::find(test.begin(), test.end(), data[i - window]);
            test.erase(found);
        }
        std::sort(test.begin(), test.end());
    }
    gettimeofday(&end, 0);
    return ((float) difftime(end.tv_sec, begin.tv_sec))
            + (((float) (end.tv_usec - begin.tv_usec)) / 100000.0);
}

/**
 * Test the speed of the double heap median filter against
 * inserting and sorting std::vectors.
 */
void speedTest(int samples, int window, std::ostream& out)
{
    float elapsed;
    std::vector<int> data;
    out << "Generating number series 0,1,-1,2,-2,...," << samples / 2
            << " for first speed test\n";
    data = divergentSeries(samples);
    out << "Testing heap with series...\n";
    elapsed = timedHeap(data, window);
    out << "Heap time: " << elapsed << " seconds\n";
    out << "Testing std::vector insert/sort with series...\n";
    elapsed = timedVector(data, window);
    out << "Vector time: " << elapsed << " seconds\n";
    out << "Generating " << samples
            << " random numbers for second speed test\n";
    data = randomSeries(samples);
    out << "Testing heap with random numbers...\n";
    elapsed = timedHeap(data, window);
    out << "Heap time: " << elapsed << " seconds\n";
    out << "Testing std::vector insert/sort with random numbers...\n";
    elapsed = timedVector(data, window);
    out << "Vector time: " << elapsed << " seconds\n";
}

/**
 *  A simple test of the running median filter.
 */
int main(char** argv, unsigned int argc)
{
    int window = 65;
    std::cout << "Using window size " << window << " for testing.\n";
    std::cout << "Testing heap accuracy.\n";
    accuracyTest(10000, window, std::cout);
    std::cout << "Testing heap speed.\n";
    speedTest(100000000, window, std::cout);
    return 0;
}
