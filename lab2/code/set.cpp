#include "set.h"
#include "node.h"

int Set::Node::count_nodes = 0;

/*****************************************************
 * Implementation of the member functions             *
 ******************************************************/

 /*
  *  Default constructor :create an empty Set
  */
// T(1) = O(1)
// S(1) = O(1)
int Set::get_count_nodes() {
	return Set::Node::count_nodes;
}

/*
 *  Default constructor :create an empty Set
 */
 // T(1) = O(1)
 // S(1) = O(1)
Set::Set() : counter{ 0 } {
	// IMPLEMENT before Lab2 HA
	head = new Node();
	tail = new Node();
	head->next = tail;
	tail->prev = head;
}

/*
 *  Conversion constructor: convert val into a singleton {val}
 */
Set::Set(int val) : Set{} {  // create an empty list
	// IMPLEMENT before Lab2 HA
	insert_node(head, val);
}

/*
 * Constructor to create a Set from a sorted vector of unique ints
 * Create a Set with all ints in sorted vector list_of_values
 */
Set::Set(const std::vector<int>& list_of_values) : Set{} {  // create an empty list
	// IMPLEMENT before Lab2 HA

	Node* temp = head;
	for (int var : list_of_values) {

		insert_node(temp, var);
		temp = temp->next;
	}
}

/*
 * Copy constructor: create a new Set as a copy of Set S
 * \param S Set to copied
 * Function does not modify Set S in any way
 */
// T(6+n) = T(n) = O(n) linear
// S(2+n) = S(n) linear
Set::Set(const Set& S) : Set{} {  // create an empty list
	// IMPLEMENT before Lab2 HA
	Node* stepPtr = S.head; // T(1) S(1)
	Node* temp = head; // T(1) S(1)
	while (stepPtr->next != S.tail) { // T(n+1) = O(n), S(n)

		stepPtr = stepPtr->next; //T(1)
		insert_node(temp, stepPtr->value); //T(1) S(1) <- built into while
		temp = temp->next; //T(1)

	}
}

/*
 * Transform the Set into an empty set
 * Remove all nodes from the list, except the dummy nodes
 */
void Set::make_empty() {
	// IMPLEMENT before Lab2 HA
	Node* mainStep = head->next;

	//mainStep->prev = nullptr;

	while (mainStep->next != this->tail && mainStep != this->tail) {
		Node* backStep = mainStep;
		mainStep = mainStep->next;

		remove_node(backStep);
	}
	if (mainStep != this->tail) remove_node(mainStep);


}

/*
 * Destructor: deallocate all memory (Nodes) allocated for the list
 */
Set::~Set() {
	// IMPLEMENT before Lab2 HA
	this->make_empty();
	delete head;
	delete tail;
}

/*
 * Assignment operator: assign new contents to the *this Set, replacing its current content
 * \param S Set to be copied into Set *this
 * Call by valued is used
 */
// T(n2+3) = T(n2) = O(n2)
// S(n2) = O(n2)
Set& Set::operator=(Set S) { //copy constructor is called for S, which is T(n2) S(n2)
	// IMPLEMENT before Lab2 HA
	// All swap will have combined S(1) will deallocate memory after running each swap 
	std::swap(counter, S.counter); // T(1) S(1)
	std::swap(head, S.head); // T(1) S(1)
	std::swap(tail, S.tail); // T(1) S(1)
	return *this; //not counting returns
}

/*
 * Test whether val belongs to the Set
 * Return true if val belongs to the set, otherwise false
 * This function does not modify the Set in any way
 */
bool Set::is_member(int val) const {
	// IMPLEMENT before Lab2 HA
	Node* temp = head->next;
	while (temp != tail) {
		if (temp->value == val) {
			return true;
		}
		temp = temp->next;
	}
	return false;
}

/*
 * Test whether Set *this and S represent the same set
 * Return true, if *this has same elemnts as set S
 * Return false, otherwise
 */
bool Set::operator==(const Set& S) const {
	return operator<=>(S) == std::partial_ordering::equivalent;
}


/*
 * Three-way comparison operator: to test whether *this == S, *this < S, *this > S
 * Return std::partial_ordering::equivalent, if *this == S
 * Return std::partial_ordering::less, if *this < S
 * Return std::partial_ordering::greater, if *this > S
 * Return std::partial_ordering::unordered, otherwise
 */
std::partial_ordering Set::operator<=>(const Set& S) const {
	// IMPLEMENT before Lab2 HA


	Node* readhead1 = this->head->next;
	Node* readhead2 = S.head->next;
	bool thisSubOfS = false;
	bool SSubOfThis = false;
	//bool thisSameS = true;

	while (readhead1 != this->tail && readhead2 != S.tail)
	{
		if (readhead1->value < readhead2->value) {
			SSubOfThis = true;
			//thisSameS = false;
			readhead1 = readhead1->next;

		}
		else if (readhead1->value > readhead2->value) {
			thisSubOfS = true;
			//thisSameS = false;
			readhead2 = readhead2->next;
		}
		else {
			readhead2 = readhead2->next;
			readhead1 = readhead1->next;
		}
	}//while

	if (readhead1 != this->tail) {
		SSubOfThis = true;
	}
	if (readhead2 != S.tail) {
		thisSubOfS = true;
	}


	if (SSubOfThis && thisSubOfS) {
		return std::partial_ordering::unordered;
	}
	else if (SSubOfThis) {
		return std::partial_ordering::greater;
	}
	else if (thisSubOfS) {
		return std::partial_ordering::less;
	}
	else {
		return std::partial_ordering::equivalent;
	}


}

/*
 * Modify Set *this such that it becomes the union of *this with Set S
 * Set *this is modified and then returned
 */
//n1 - length of *this, n2 - length of S
//T(n2,n1) = O(n1,n2) will loop though the entire n2 but not always n1.
// S(n2) = O(n2) if there is something in s2 we will insert otherwise there will be no difference in the space.
Set& Set::operator+=(const Set& S) {
	// IMPLEMENT
	Node* readhead1 = this->head->next; // S(1) T(1)
	Node* readhead2 = S.head->next; // S(1) T(1)
	while (readhead1 != this->tail && readhead2 != S.tail){ // T(min(n1, n2)+3) = O(n1, n2), S(min(n1,n2)) = S(n2) = O(n2) because insert_node
		if (readhead1->value < readhead2->value) { // T(3)
			readhead1 = readhead1->next; 

		}
		else if (readhead1->value > readhead2->value) { // T(3)
			insert_node(readhead1->prev, readhead2->value); //T(1) S(1)
			readhead2 = readhead2->next; // T(1)
		}
		else {
			readhead2 = readhead2->next; //T(1)
			readhead1 = readhead1->next; //T(1)
		}


	} // while

	while (readhead2 != S.tail) { //T(n2) S(n2 - min(n1,n2))
		insert_node(readhead1->prev, readhead2->value); // T(1) S(1)
		readhead2 = readhead2->next; //T(1)
	}
	return *this;
} 

/*
 * Modify Set *this such that it becomes the intersection of *this with Set S
 * Set *this is modified and then returned
 */
// T(n1,n2) = O(n1,n2) linear func of n1 and n2
// S(3) = S(1) = O(1) linear
Set& Set::operator*=(const Set& S) {
	// IMPLEMENT
	Node* readhead1 = this->head->next; //T(1) S(1)
	Node* readhead2 = S.head->next; //T(1) S(1)
	while (readhead1 != this->tail && readhead2 != S.tail) //T(min(n1, n2)) S(1)
	{
		if (readhead1->value < readhead2->value) {
			Node* temp = readhead1; // T(1) S(1)
			readhead1 = readhead1->next;
			remove_node(temp); //T(1)
		}
		else if (readhead1->value > readhead2->value) {

			readhead2 = readhead2->next;
		}
		else {
			readhead2 = readhead2->next;
			readhead1 = readhead1->next;
		}

	}
		while (readhead1 != this->tail) // T(n1 - min(n1, n2)) S(1)
		{
			Node* temp = readhead1; // S(1) T(1)
			readhead1 = readhead1->next;
			remove_node(temp); // T(1)
		}

	return *this;
}

/*
 * Modify Set *this such that it becomes the Set difference between Set *this and Set S
 * Set *this is modified and then returned
 */
Set& Set::operator-=(const Set& S) {
	// IMPLEMENT
	Node* readhead1 = this->head->next;
	Node* readhead2 = S.head->next;
	while (readhead1 != this->tail && readhead2 != S.tail)
	{
		if (readhead1->value < readhead2->value) {
			readhead1 = readhead1->next;

		}
		else if (readhead1->value > readhead2->value) {
			//insert_node(readhead1, readhead2->value);
			readhead2 = readhead2->next;
		}
		else {
			Node* temp = readhead1;
			readhead1 = readhead1->next;
			readhead2 = readhead2->next;
			remove_node(temp);
			//readhead1 = readhead1->next;
		}


	} // while
	return *this;
}


/* ******************************************** *
 * Private Member Functions -- Implementation   *
 * ******************************************** */

 /*
  * Insert a new Node storing val after the Node pointed by p
  * \param p pointer to a Node
  * \param val value to be inserted  after position p
  */
  // T(1) = O(1)
  // S(1) = O(1)
void Set::insert_node(Node* p, int val) {
	// IMPLEMENT before Lab2 HA
	Node* newNode = new Node(val, p->next, p);

	p->next->prev = newNode;
	p->next = newNode;
	counter++;
}

/*
 * Remove the Node pointed by p
 * \param p pointer to a Node
 */
void Set::remove_node(Node* p) { //T(1) = O(1) 
	// IMPLEMENT before Lab2 HA
	counter--;
	p->prev->next = p->next;
	p->next->prev = p->prev;
	delete p; //T(1)
}

/*
 * Write Set *this to stream os
 */
void Set::write_to_stream(std::ostream& os) const {
	if (is_empty()) {
		os << "Set is empty!";
	}
	else {
		Set::Node* ptr{ head->next };

		os << "{ ";
		while (ptr != tail) {
			os << ptr->value << " ";
			ptr = ptr->next;
		}
		os << "}";
	}
}
