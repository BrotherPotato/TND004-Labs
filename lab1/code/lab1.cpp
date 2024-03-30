// lab1.cpp : stable partition
// Iterative and divide-and-conquer

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <format>
#include <functional>
#include <cassert>


/****************************************
 * Declarations                          *
 *****************************************/

// generic class to write an item to a stream
template <typename T>
class Formatter {
public:
    Formatter(std::ostream& os, int width, int per_line)
        : os_{os}, per_line_{per_line}, width_{width} {
    }

    void operator()(const T& t) {
        os_ << std::format("{:{}}", t, width_);
        if (++outputted_ % per_line_ == 0)
            os_ << "\n";
    }

private:
    std::ostream& os_;    // output stream
    const int per_line_;  // number of columns per line
    const int width_;     // column width
    int outputted_{0};    // counter of number of items written to os_
};

/* ************************ */

namespace TND004 {
// Iterative algorithm
void stable_partition_iterative(std::vector<int>& V, std::function<bool(int)> p);

// Auxiliary function that performs the stable partition recursively
std::vector<int>::iterator stable_partition(std::vector<int>::iterator first,
                                            std::vector<int>::iterator last,
                                            std::function<bool(int)> p);

// Divide-and-conquer algorithm
void stable_partition(std::vector<int>& V, std::function<bool(int)> p) {
    TND004::stable_partition(std::begin(V), std::end(V), p);  // call auxiliary function
}
}  // namespace TND004

void execute(std::vector<int>& V, const std::vector<int>& res);

bool even(int i);

/****************************************
 * Main:test code                        *
 *****************************************/

int main() {
    /*****************************************************
     * TEST PHASE 1                                       *
     ******************************************************/
    {
        std::cout << "TEST PHASE 1\n\n";

        std::vector<int> seq{1, 2};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>{std::cout, " "});

        execute(seq, std::vector<int>{2, 1});

        std::cout << "\nEmpty sequence: ";
        std::vector<int> empty;

        execute(empty, std::vector<int>{});
    }

    /*****************************************************
     * TEST PHASE 2                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 2\n\n";

        std::vector<int> seq{2};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>{std::cout, " "});

        execute(seq, std::vector<int>{2});
    }

    /*****************************************************
     * TEST PHASE 3                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 3\n\n";

        std::vector<int> seq{3};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>{std::cout, " "});

        execute(seq, std::vector<int>{3});
    }

    /*****************************************************
     * TEST PHASE 4                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 4\n\n";

        std::vector<int> seq{3, 3};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>(std::cout, " "));

        execute(seq, std::vector<int>{3, 3});
    }

    /*****************************************************
     * TEST PHASE 5                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 5\n\n";

        std::vector<int> seq{1, 2, 3, 4, 5, 6, 7, 8, 9};

        std::cout << "Sequence: ";
        std::copy(std::begin(seq), std::end(seq), std::ostream_iterator<int>(std::cout, " "));

        execute(seq, std::vector<int>{2, 4, 6, 8, 1, 3, 5, 7, 9});
    }

    /*****************************************************
     * TEST PHASE 6                                       *
     ******************************************************/
    {
        std::cout << "\n\nTEST PHASE 6: test with long sequence loaded from a file\n\n";

        std::ifstream file("../code/test_data.txt"); // if mac then change this path

        if (!file) {
            std::cout << "Could not open test_data.txt!!\n";
            return 0;
        }

        // read the input sequence from file
        std::vector<int> seq{std::istream_iterator<int>{file}, std::istream_iterator<int>()};
        file.close();

        std::cout << "\nNumber of items in the sequence: " << std::ssize(seq) << '\n';

        /*std::cout << "Sequence:\n";
        std::for_each(std::begin(seq), std::end(seq), Formatter<int>(std::cout, 8, 5));*/

        // read the result sequence from file
        file.open("../code/test_result.txt");  // if mac then change this path

        if (!file) {
            std::cout << "Could not open test_result.txt!!\n";
            return 0;
        }

        std::vector<int> res{std::istream_iterator<int>{file}, std::istream_iterator<int>()};

        std::cout << "\nNumber of items in the result sequence: " << std::ssize(res);

        // display expected result sequence
        // std::for_each(std::begin(res), std::end(res), Formatter<int>(std::cout, 8, 5));

        assert(std::ssize(seq) == std::ssize(res));

        execute(seq, res);
    }
}

/****************************************
 * Functions definitions                 *
 *****************************************/

bool even(int i) {
    return i % 2 == 0;
}

// Used for testing
void execute(std::vector<int>& V, const std::vector<int>& res) {
    std::vector<int> copy_{V};

    std::cout << "\n\nIterative stable partition\n";
    TND004::stable_partition_iterative(V, even);
    assert(V == res);  // compare with the expected result

    
    // Uncomment for exercise 2
    std::cout << "Divide-and-conquer stable partition\n";
    TND004::stable_partition(copy_, even);
    assert(copy_ == res);  // compare with the expected result
    
}

// Iterative algorithm
void TND004::stable_partition_iterative(std::vector<int>& V, std::function<bool(int)> p) {
    // IMPLEMENT before Lab1 HA

    std::vector<int> trueVector;  // O(1)   T(0)?
    std::vector<int> falseVector; // O(1)   T(0)?

	//malloc using reserve to avoid reallocation? and make the code faster. 
    //https://en.cppreference.com/w/cpp/container/vector/reserve
    trueVector.reserve(std::ssize(V));
    falseVector.reserve(std::ssize(V));
    
    // whole loop O(n) T(1 + n(1+1+1+(1)+(1 or 1))) = T(i + loop(comp, size, incr, (if), (pushback) ))
    for (size_t i = 0; i < std::ssize(V); i++) // O(n)
    {
        if (p(V[i])) { // O(1 or n)  T(1+1)??
            trueVector.push_back(V[i]); // O(1)  T(n)
            //trueVector[0] = V[i]; // O(1)  T(n)
        }
        else {
            falseVector.push_back(V[i]); // O(1) T(n)
            //falseVector[0] = V[i]; // O(1)  T(n)
        }
    }
    
    // complexity 4) Linear in std::distance(first, last) plus linear in the distance between pos and end of the container.
    // T(n+n+c) = T(2n) = O(n) c är en konstant
    trueVector.insert(trueVector.end(), falseVector.begin(), falseVector.end()); // constant time? bra fråga
    
    //trueVector.append_range(falseVector); //same thing as above O(n) 
    
    // std::back_inserter returns std::back_insert_iterator that uses Container::push_back().
    // copy is of time complexity O(n), back_inserter O(1+1+n) = O(n)
    //copy(falseVector.begin(), falseVector.end(), std::back_inserter(trueVector));  //same thing as above  // T(2n) = O(n)
    V = trueVector; // O(1) T(1)
    //Whole function is O(n)
}

// Auxiliary function that performs the stable partition recursively
// Divide-and-conquer algorithm: stable-partition the sub-sequence starting at first and ending
// at last-1. If there are items with property p then return an iterator to the end of the block
// containing the items with property p. If there are no items with property p then return first.
std::vector<int>::iterator TND004::stable_partition(std::vector<int>::iterator first,
                                                    std::vector<int>::iterator last,
                                                    std::function<bool(int)> p) {
    // IMPLEMENT

    /*
    vector<int>::iterator it1 = begin(V) + 1;
    vector<int>::iterator it2 = begin(V) + 3;
    vector<int>::iterator it3 = begin(V) + 6;
    auto it4 = std::rotate(it1, it2, it3); // rotate the range [it1, it2, it3] so that it2 becomes the first element [it2, it1, it3]?
    */
    std::vector<int>::iterator mid = first + std::distance(first, last) / 2;

    if(first == last) return first; // O(1) T(1)
	
    
    if (std::distance(first, last) == 2) { //[1, 2]
		if (p(*mid)) { //if mid is even
			return mid; // add +1??? yeeees :)) då om mid == last så kommer rotate inte modifiera vektorn
        }
        else {
			return mid;
		}
    }

    std::vector<int>::iterator SL = stable_partition(first, mid, p);
    std::vector<int>::iterator SR = stable_partition(mid, last, p);
    return std::rotate(SL, mid, SR);
}
