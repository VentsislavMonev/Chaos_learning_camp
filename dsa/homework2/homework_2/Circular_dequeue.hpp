#ifndef CIRCULAR_DEQUEUE_HPP
#define CIRCULAR_DEQUEUE_HPP

#include <iostream>
// #include "Utils.hpp"

class Circular_dequeue
{
private:
    int* data;

    int head;
    int tail;
    int size;
    int capacity;

public:
    // construction

    Circular_dequeue();
    Circular_dequeue(int _capacity);
    Circular_dequeue(const Circular_dequeue& other);
    Circular_dequeue& operator=(const Circular_dequeue& other);
    Circular_dequeue(Circular_dequeue&& other) noexcept;
    Circular_dequeue& operator=(Circular_dequeue&& other) noexcept;
    ~Circular_dequeue();

    // funcions for the user

    bool enqueue_head(int number);
    bool enqueue_tail(int number);

    void pop_head();
    void pop_tail();

    int peak_head();
    int peak_tail();

    // operator to get the value of element at index 
    int operator[](int index) const;

    int get_size()      const;
    int get_capacity()  const;
    bool is_full()      const;
    bool is_empty()     const;

    // inserts an element inside the circular dequeue at index
    // shifts the elements and leaves an empty space at rank n
    // where we add the new value
    void insert(int index, int value);

    // remove an element inside the circular dequeue at index
    void remove_at(int index);
};


#endif