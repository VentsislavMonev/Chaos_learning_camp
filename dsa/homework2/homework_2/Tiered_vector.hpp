#ifndef TIERED_VECTOR
#define TIERED_VECTOR

#include "Circular_dequeue.hpp"

class Tiered_vector
{
private:
    Circular_dequeue* tiers;
    int l;   //number of tiers and the size of a tier
    int t;   //number of tiers
    int n;   //number of elements in the tiered vector

    // get the index of the circular dequeue i have to insert in
    int tier_index(int rank) const { return rank / l; }
    // get the index in the circular dequeue i have to insert in
    int slot_index(int rank) const { return rank % l; }

public:
    // construction

    Tiered_vector(int k);           // l = 2^k, allocates l tiers
    Tiered_vector(const Tiered_vector& other);
    Tiered_vector& operator=(const Tiered_vector& other);
    Tiered_vector(Tiered_vector&& other) noexcept;
    Tiered_vector& operator=(Tiered_vector&& other) noexcept;
    ~Tiered_vector();

    // functions for the user

    int  operator[](int rank) const;    
    void insert(int rank, int value);   
    void remove(int rank);              
    void push_back(int value);          
    void pop_back();                    

    int  size()     const { return n; }
    int  capacity() const { return l * t; }
    bool empty()    const { return n == 0; }

    // Approximate memory in bytes
    int memory_bytes() const;
    
private:
    // helper functions for insert and remove

    void pop_push(int top, int end);
    void push_pop(int top, int end);

    void expand();   // build a new structure with l' = 2l, migrate elements
    void shrink();   // opposite

};

#endif