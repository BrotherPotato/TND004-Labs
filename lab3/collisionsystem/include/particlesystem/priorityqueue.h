#pragma once

#include <iostream>
#include <vector>
#include <cassert>

//#define TEST_PRIORITY_QUEUE

/**
 * A heap based priority queue where the root is the smallest element -- min heap
 */
template <class Comparable>
class PriorityQueue {
public:
    /**
     * Constructor to create a queue with the given capacity
     */
    explicit PriorityQueue(int initCapacity = 100) : orderOK{true} {
        pq.reserve(initCapacity + 1);
        makeEmpty();
        assert(isEmpty());
    }

    /**
     * Constructor to initialize a priority queue based on a given vector V
     * Assumes V[0] is not used
     */
    explicit PriorityQueue(const std::vector<Comparable>& V) : pq{V} {
        heapify();
#ifdef TEST_PRIORITY_QUEUE
        assert(isMinHeap());
#endif
    }

    /**
     * Make the queue empty
     */
    void makeEmpty() {
        pq.clear();
        pq.push_back(Comparable{});
    }

    /**
     * Check is the queue is empty
     * Return true if the queue is empty, false otherwise
     */
    bool isEmpty() const {
        return pq.size() == 1;  // slot zero is not used
    }

    /**
     * Get the size of the queue, i.e. number of elements in the queue
     */
    size_t size() const { return pq.size() - 1; }

    /**
     * Get the smallest element in the queue
     */
    Comparable findMin() {
        assert(isEmpty() == false);
        return pq[1];
    }

    /**
     * Remove and return the smallest element in the queue
     */
    Comparable deleteMin();

    /**
     * Add a new element x to the queue
     */
    void insert(const Comparable& x);

    /**
     * Insert element x in the end of the queue, without preserving the heap property
     */
    void toss(const Comparable& x);

private:
    std::vector<Comparable> pq;  // slot with index 0 not used
    bool orderOK;  // flag to keep internal track of when the heap is ordered / not ordered

    // Auxiliary member functions

    /**
     * Restore the heap-ordering property
     */
    void heapify();

    void percolateDown(size_t i);

    void percolateUp();

    /**
     * Test whether pq is a min heap
     */
    bool isMinHeap() const {
        // TO BE IMPLEMENTED
        for (size_t i = 1; i < this->size(); i++) {

            if (this->size() == 1) return true; 

            bool isLeafL = (2 * i) > this->size();
            if (isLeafL) break;  // stops when first leaf is hit
            auto leftChild = 2 * i;
            if (pq[i] > pq[leftChild]) return false;

            bool isLeafR = 2 * i + 1 > this->size();
            if (isLeafR) break;
            auto rightChild = leftChild + 1;
            if (pq[i] > pq[rightChild]) return false;

        }
       
        return true;  // delete this line
    }
};

template <class Comparable>
void PriorityQueue<Comparable>::percolateDown(size_t i) {
    Comparable temp = pq[i];
    auto c = 2 * i;  // left child

    while (c < pq.size()) {
        if (c < pq.size() - 1) {
            if (pq[c + 1] < pq[c])  // largest child?
                c++;
        }
        // percolate down
        if (pq[c] < temp) {
            pq[i] = pq[c];
            i = c;
            c = 2 * i;
        } else {
            break;
        }
    }
    pq[i] = temp;
}

template <class Comparable>
void PriorityQueue<Comparable>::percolateUp() {
    int child = pq.size()-1;
    int parent = child / 2;

    while (parent > 0) {
        if (pq[child] < pq[parent]) {
            std::swap(pq[child], pq[parent]);
            child = parent;
            parent = child/2;
        } else break;
    }
}

/**
 * Restore the heap property
 */
template <class Comparable>
void PriorityQueue<Comparable>::heapify() {
    assert(pq.size() > 1);  // slot zero is not used

    for (size_t i = (pq.size() - 1) / 2; i >= 1; --i) {
        percolateDown(i);
    }
    orderOK = true;
}

/**
 * Remove and return the smallest element in the queue
 */
template <class Comparable>
Comparable PriorityQueue<Comparable>::deleteMin() {
    assert(!isEmpty());

    if (!orderOK) {
        heapify();
    }

    Comparable x = pq[1];
    Comparable y = pq[pq.size() - 1];

    pq[1] = y;  // set last element in the heap as the new root
    percolateDown(1);
    pq.pop_back();

#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
    return x;
}

/**
 * Insert element x on the last slot, without preserving the heap property
 */
template <class Comparable>
void PriorityQueue<Comparable>::toss(const Comparable& x) {
    orderOK = false;
    pq.push_back(x);
}

/**
 * Add a new element x to the queue
 */
template <class Comparable>
void PriorityQueue<Comparable>::insert(const Comparable& x) {
    // TO BE IMPLEMENTED


    //toss(x);  // delete this line
    pq.push_back(x);
    percolateUp();
    orderOK = true;



#ifdef TEST_PRIORITY_QUEUE  // do not delete
    assert(isMinHeap());
#endif
}
