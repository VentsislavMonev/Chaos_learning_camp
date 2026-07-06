#include "Circular_dequeue.hpp"

int mod(int number, int module)
{
    return (number % module + module) % module;
}

Circular_dequeue::Circular_dequeue()
    : head(0), tail(0), size(0), capacity(1)
{
    data = new int[capacity];  
}

Circular_dequeue::Circular_dequeue(int _capacity)
    : head(0), tail(0), size(0), capacity(_capacity)
{ 
    data = new int[capacity];
}

Circular_dequeue::Circular_dequeue(const Circular_dequeue &other)
    : head(other.head), tail(other.tail), size(other.size), capacity(other.capacity)
{
    data = new int[capacity];
    std::copy(other.data, other.data + capacity, data);
}

Circular_dequeue &Circular_dequeue::operator=(const Circular_dequeue &other)
{
     if (this != &other) 
     {
        Circular_dequeue temp(other);

        std::swap(data,temp.data);
        std::swap(head,temp.head);
        std::swap(tail,temp.tail);
        std::swap(size,temp.size);
        std::swap(capacity, temp.capacity);
    }
    return *this;
}

Circular_dequeue::Circular_dequeue(Circular_dequeue &&other) noexcept : data(other.data), head(other.head), tail(other.tail), size(other.size), capacity(other.capacity)
{
    other.data = nullptr;
    other.head = 0;
    other.tail = 0;
    other.size = 0;
    other.capacity = 0;
}

Circular_dequeue &Circular_dequeue::operator=(Circular_dequeue &&other) noexcept
{
    if (this != &other) 
    {
        delete[] data;
        data = other.data;
        head = other.head;
        tail = other.tail;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.head = 0;
        other.tail = 0;
        other.size = 0;
        other.capacity = 0;
    }
    return *this;
}

Circular_dequeue::~Circular_dequeue()
{
    delete[] data;
}

bool Circular_dequeue::enqueue_head(int number)
{
    if(size==capacity)
    {
        return false;
    }
    else if (size==0)
    {
        data[head]=number;
        ++size;
        return true;
    }
    else
    {
        head=mod(head-1, capacity);
        data[head]=number;
        ++size;
        return true;
    }   
}

bool Circular_dequeue::enqueue_tail(int number)
{
    if(size==capacity)
    {
        return false;
    }
    else if (size==0)
    {
        data[tail]=number;
        ++size;
        return true;
    }
    else
    {
        tail=mod(tail+1, capacity);
        data[tail]=number;
        ++size;
        return true;
    }
}

void Circular_dequeue::pop_head()
{
    if(size==0)
    {
        return;
    }
    else if (size==1)
    {        
        --size;
    }
    else if(size!=0)
    {
        head=mod(head+1, capacity);
        --size;
    }
}

void Circular_dequeue::pop_tail()
{
    if(size==0)
    {
        return;
    }
    else if (size==1)
    {        
        --size;
    }
    else if(size!=0)
    {
        tail=mod(tail-1, capacity);
        --size;
    }
}

int Circular_dequeue::peak_head()
{
    if(size==0) throw std::out_of_range("Circular_dequeue::peak_head: index out of range!");
    return data[head];
}

int Circular_dequeue::peak_tail()
{
    if(size==0) throw std::out_of_range("Circular_dequeue::peak_tail: index out of range!");
    return data[tail];
}

int Circular_dequeue::operator[](int index) const
{
    if(index<0 || index>=size) throw std::out_of_range("Circular_dequeue::operator[]: index out of range!");
    return data[mod(head + index, capacity)];
}

int Circular_dequeue::get_size() const
{
    return size;
}

int Circular_dequeue::get_capacity() const
{
    return capacity;
}

bool Circular_dequeue::is_full() const
{
    return size==capacity;
}

bool Circular_dequeue::is_empty() const
{
    return size==0;
}

void Circular_dequeue::insert(int index, int value)
{
    if (size == capacity) throw std::overflow_error("Circular_dequeue::insert: cant insert any more elements");
    if (index<0 || index > size) throw std::out_of_range("Circular_dequeue::insert: index out of range");

    if (size == 0)
    {
        data[head] = value;
        ++size;
        return;
    }

    // we shift the elements to the left here and increase the size
    head = mod(head - 1, capacity);
    for (int i = 0; i < index; ++i)
    {
        data[mod(head + i, capacity)] = data[mod(head + i + 1, capacity)];
    }
    ++size;

    // Size was already incremented inside and tail or head were moved
    data[mod(head + index, capacity)] = value;
}

void Circular_dequeue::remove_at(int index)
{

    if (size == 0) throw std::out_of_range("Circular_dequeue: dequeue is empty!");
    else if (index < 0 || index >= size) throw std::out_of_range("Circular_dequeue: invalid index to remove!");

    for (int i = index; i < size-1; i++)
    {
        data[mod(head+i, capacity)] = data[mod(head+i+1, capacity)];
    }
    tail = mod(tail-1, capacity);
    --size;
}