#include "Tiered_vector.hpp"

Tiered_vector::Tiered_vector(int k) 
    :l(1<<k), t(1<<k), n(0)
{
    tiers = new Circular_dequeue[t];
    for (int i = 0; i < t; ++i)
    {
        tiers[i] = Circular_dequeue(l);
    }
}

Tiered_vector::Tiered_vector(const Tiered_vector &other)
    : l(other.l), t(other.t), n(other.n)
{
    tiers = new Circular_dequeue[t];
    for (int i = 0; i < t; ++i)
    {
        tiers[i]=other.tiers[i];
    }
}

Tiered_vector &Tiered_vector::operator=(const Tiered_vector &other)
{
    if(this==&other) return *this;

    Tiered_vector temp(other);
    std::swap(tiers, temp.tiers);
    std::swap(l, temp.l);
    std::swap(t, temp.t);
    std::swap(n, temp.n);

    return *this;
}

Tiered_vector::Tiered_vector(Tiered_vector &&other) noexcept : tiers(other.tiers), l(other.l), t(other.t), n(other.n)
{
    other.tiers = nullptr;
    other.n = 0;
}

Tiered_vector &Tiered_vector::operator=(Tiered_vector &&other) noexcept
{
    if (this == &other) return *this;

    delete[] tiers;
    
    tiers = other.tiers;
    l = other.l;
    t = other.t;
    n = other.n;

    other.tiers = nullptr;
    other.n = 0;

    return *this;
}

Tiered_vector::~Tiered_vector()
{
    if(tiers) delete[] tiers;
}

int Tiered_vector::operator[](int rank) const
{
    if (rank < 0 || rank >= n) throw std::out_of_range("Tiered_vector::operator[]: rank out of range!");
    int ti = tier_index(rank);
    int si = slot_index(rank);
    return tiers[ti][si];
}


void Tiered_vector::insert(int rank, int value)
{
    if (rank<0 || rank > n) throw std::out_of_range("Tiered_vector::insert: rank out of range");
    
    if(n==l*t) expand();                // expand the vector if needed

    int tier_top = tier_index(rank);    // dequeue where rank is
    int tier_end = tier_index(n);       // dequeue where the next new element

    pop_push(tier_top, tier_end);       // pop push so we have space for the new one

    tiers[tier_top].insert(slot_index(rank),value);
    ++n;
}

void Tiered_vector::remove(int rank)
{
    if (rank<0 || rank >= n) throw std::out_of_range("Tiered_vector::Remove: rank out of range");
 
    int tier_top = tier_index(rank);    // dequeue where rank is
    int tier_end = tier_index(n - 1);   // dequeue where the last element is
    int slot = slot_index(rank);
 
    tiers[tier_top].remove_at(slot);    // remove within the target tier
 
    push_pop(tier_top, tier_end);       // pull the head of each subsequent tier back to fill the gap
    --n;
 
    if (n == (l * l) / 8) shrink();     // we shrink if we reached only 1/8 of the elements because
                                        // otherwise if we need to insert we will expand again right after shrink
}

void Tiered_vector::push_back(int value)
{
    insert(n,value);
}

void Tiered_vector::pop_back()
{
    if(n==0) return;
    tiers[tier_index(n-1)].pop_tail();
    --n;
}

void Tiered_vector::pop_push(int top, int end)
{
    if(top==end) return;

    int element = 0;
    for (int i = end-1; i >=top; --i)
    {
        element = tiers[i].peak_tail();
        tiers[i].pop_tail();
        tiers[i+1].enqueue_head(element);
    }
}

void Tiered_vector::push_pop(int top, int end)
{
    if(top==end) return;

    for (int i = top; i < end; ++i) {
        int elem = tiers[i + 1].peak_head();
        tiers[i + 1].pop_head();
        tiers[i].enqueue_tail(elem);
    }
}

void Tiered_vector::expand()
{
    int new_l = l*2;
    int new_t = new_l;

    Circular_dequeue* new_tiers = new Circular_dequeue[new_t];

    for (int i = 0; i < new_t; ++i)
        new_tiers[i] = Circular_dequeue(new_l);    

    for (int i = 0; i < t/2; ++i)
    {
        Circular_dequeue& first = tiers[2*i];
        Circular_dequeue& second = tiers[2*i+1];
        Circular_dequeue& new_tier = new_tiers[i];

        for (int j = 0; j < first.get_size(); ++j)
        {
            new_tier.enqueue_tail(first[j]);
        }

        for (int j = 0; j < second.get_size(); ++j)
        {
            new_tier.enqueue_tail(second[j]);
        }
    }

    delete[] tiers;
    tiers=new_tiers;
    l=new_l;
    t=new_t;
}

void Tiered_vector::shrink()
{
    int new_l = l / 2;
    int new_t = new_l;

    Circular_dequeue* new_tiers = new Circular_dequeue[new_t];
    for (int i = 0; i < new_t; ++i)
        new_tiers[i] = Circular_dequeue(new_l);

    for (int i = 0; i < new_t / 2; ++i)
    {
        Circular_dequeue& old_tier = tiers[i];
        Circular_dequeue& left = new_tiers[2 * i];
        Circular_dequeue& right = new_tiers[2 * i + 1];

        int old_size = old_tier.get_size();

        for (int j = 0; j < old_size && j < new_l; ++j)
            left.enqueue_tail(old_tier[j]);

        for (int j = new_l; j < old_size; ++j)
            right.enqueue_tail(old_tier[j]);
    }

    delete[] tiers;
    tiers = new_tiers;
    l = new_l;
    t = new_t;
}

int Tiered_vector::memory_bytes() const
{
    return sizeof(*this) + t * sizeof(Circular_dequeue) + t * l * sizeof(int);
}