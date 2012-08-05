#ifndef LINKED_LIST_HPP_INCLUDED
#define LINKED_LIST_HPP_INCLUDED

/**
 * A simple singly-linked list class.
 */
template<class T>
class LinkedList {
private:
    /**
     * The value of this node.
     */
    T value;
    /**
     * The following node in the list.
     */
    LinkedList<T>* next;
public:
    LinkedList(T value2) {
        value = value2;
        next = NULL;
    }
    
    LinkedList(T value2, LinkedList* next2) {
        value = value2;
        next = next2;
    }
    
    T getValue() {
        return value;
    }
    
    LinkedList<T>* getNext() {
        return next;
    }
    
    void setNext(LinkedList<T>* next2) {
        next = next2;
    }
};

#endif
