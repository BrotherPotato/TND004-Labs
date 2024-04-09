#include "set.h"
#include "node.h"

int Set::Node::count_nodes = 0;

/*****************************************************
 * Implementation of the member functions             *
 ******************************************************/

 /*
  *  Default constructor :create an empty Set
  */
int Set::get_count_nodes() {
	return Set::Node::count_nodes;
}

/*
 *  Default constructor :create an empty Set
 */
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
	//Node* firstNode = new Node(val, tail, head); // the first constructor in node.h
	insert_node(head, val);
	/*   head->next = firstNode;
	   tail->prev = firstNode;*/
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

		//Node* newLastNode = new Node(var, tail, tail->prev); // set newLastNode next and prev
		//(tail->prev)->next = newLastNode; // the previous last nodes next is set to newLastNode
		//tail->prev = newLastNode; // the tails prev is set to newLastNode
	}
}

/*
 * Copy constructor: create a new Set as a copy of Set S
 * \param S Set to copied
 * Function does not modify Set S in any way
 */
Set::Set(const Set& S) : Set{} {  // create an empty list
	// IMPLEMENT before Lab2 HA
	Node* stepPtr = S.head;
	Node* temp = head;
	while (stepPtr->next != S.tail) {


		//Node* newLastNode = new Node(stepPtr->value, tail, tail->prev); 
		//(tail->prev)->next = newLastNode; 
		//tail->prev = newLastNode;
		stepPtr = stepPtr->next;

		insert_node(temp, stepPtr->value);
		temp = temp->next;

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

		//backStep->next = nullptr;
		//mainStep->prev = nullptr;
		remove_node(backStep);
		//delete backStep;
	}

	//mainStep->next = nullptr;
	//delete mainStep;
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
Set& Set::operator=(Set S) {
	// IMPLEMENT before Lab2 HA
	std::swap(counter, S.counter);
	std::swap(head, S.head);
	std::swap(tail, S.tail);
	return *this;
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
	return false;  // remove this line  //???? no
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
	//if (*this == S) {
	//    return std::partial_ordering::equivalent;
	//}

	//return std::partial_ordering::unordered; // remove this line


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
Set& Set::operator+=(const Set& S) {
	// IMPLEMENT
	Node* readhead1 = this->head->next;
	Node* readhead2 = S.head->next;
	while (readhead1 != this->tail && readhead2 != S.tail)
	{
		if (readhead1->value < readhead2->value) {
			readhead1 = readhead1->next;

		}
		else if (readhead1->value > readhead2->value) {
			insert_node(readhead1->prev, readhead2->value);
			readhead2 = readhead2->next;
		}
		else {
			readhead2 = readhead2->next;
			readhead1 = readhead1->next;
		}


	} // while

	while (readhead2 != S.tail) {
		insert_node(readhead1->prev, readhead2->value);
		readhead2 = readhead2->next;
	}
	return *this;
} // O(2n) = O(n)

/*
 * Modify Set *this such that it becomes the intersection of *this with Set S
 * Set *this is modified and then returned
 */
Set& Set::operator*=(const Set& S) {
	// IMPLEMENT
	Node* readhead1 = this->head->next;
	Node* readhead2 = S.head->next;
	while (readhead1 != this->tail && readhead2 != S.tail)
	{
		if (readhead1->value < readhead2->value) {
			Node* temp = readhead1;
			readhead1 = readhead1->next;
			remove_node(temp);
		}
		else if (readhead1->value > readhead2->value) {

			readhead2 = readhead2->next;
		}
		else {
			readhead2 = readhead2->next;
			readhead1 = readhead1->next;
		}


	}
	//if (readhead2 != S.tail) {
	//	while (readhead1 != this->tail)
	//	{
	//		Node* temp = readhead1;
	//		readhead1 = readhead1->next;
	//		remove_node(temp);
	//	}
	//}
	if (readhead1 != this->tail) {
		while (readhead1 != this->tail)
		{
			Node* temp = readhead1;
			readhead1 = readhead1->next;
			remove_node(temp);
		}
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


	}
	//if (readhead1 == this->tail) {
	//	while (readhead2 != S.tail) {
	//		insert_node(readhead1, readhead2->value);
	//	}
	//}
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
void Set::insert_node(Node* p, int val) {
	// IMPLEMENT before Lab2 HA
	Node* newNode = new Node(val, p->next, p);

	//newNode->prev = p;
	//newNode->next = p->next;
	p->next->prev = newNode;
	p->next = newNode;
	
	counter++;
	//p->prev = p->prev->next = newNode; 
}

/*
 * Remove the Node pointed by p
 * \param p pointer to a Node
 */
void Set::remove_node(Node* p) {
	// IMPLEMENT before Lab2 HA
	counter--;
	p->prev->next = p->next;
	p->next->prev = p->prev;
	delete p;
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
