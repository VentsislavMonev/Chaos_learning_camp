#include <iostream>
#include <cassert>
#include <string>
#include "Circular_dequeue.hpp"

// ─── helpers ────────────────────────────────────────────────────────────────

static int passed = 0, failed = 0;

void check(bool condition, const std::string& label)
{
    if (condition) { std::cout << "  [PASS] " << label << "\n"; ++passed; }
    else           { std::cout << "  [FAIL] " << label << "\n"; ++failed; }
}

void section(const std::string& title)
{
    std::cout << "\n== " << title << " ==\n";
}

// ─── test suites ─────────────────────────────────────────────────────────────

void test_construction()
{
    section("Construction");
    Circular_dequeue dq(4);
    check(dq.get_capacity() == 4,  "capacity is 4");
    check(dq.get_size()     == 0,  "size starts at 0");
    check(dq.is_empty(),           "empty on construction");
    check(!dq.is_full(),           "not full on construction");
}

void test_enqueue_tail_and_index()
{
    section("Enqueue-tail + operator[]");
    Circular_dequeue dq(4);
    dq.enqueue_tail(10);
    dq.enqueue_tail(20);
    dq.enqueue_tail(30);

    check(dq.get_size() == 3,   "size is 3 after 3 tail enqueues");
    check(dq[0] == 10,          "[0] == 10");
    check(dq[1] == 20,          "[1] == 20");
    check(dq[2] == 30,          "[2] == 30");
    check(dq.peak_head() == 10, "peak_head == 10");
    check(dq.peak_tail() == 30, "peak_tail == 30");
}

void test_enqueue_head()
{
    section("Enqueue-head");
    Circular_dequeue dq(4);
    dq.enqueue_tail(20);
    
    check(dq.peak_head() == 20, "peak_head == 20");
    check(dq.peak_tail() == 20, "peak_tail == 20");

    dq.enqueue_head(10);   // wraps head backwards
    dq.enqueue_tail(30);

    // logical order should be: 10, 20, 30
    check(dq.get_size() == 3,   "size is 3");
    check(dq[0] == 10,          "[0] == 10 (head-enqueued)");
    check(dq[1] == 20,          "[1] == 20");
    check(dq[2] == 30,          "[2] == 30");
}

void test_full_and_overflow()
{
    section("Full / overflow");
    Circular_dequeue dq(3);
    dq.enqueue_tail(1);
    dq.enqueue_tail(2);
    dq.enqueue_tail(3);

    check(dq.is_full(),                  "full after 3 enqueues into capacity-3");
    check(!dq.enqueue_tail(99),          "enqueue_tail on full returns false");
    check(!dq.enqueue_head(99),          "enqueue_head on full returns false");
    check(dq.get_size() == 3,            "size unchanged after failed enqueues");
}

void test_pop_head()
{
    section("Pop-head");
    Circular_dequeue dq(4);
    dq.enqueue_tail(1);
    dq.enqueue_tail(2);
    dq.enqueue_tail(3);

    dq.pop_head();
    check(dq.get_size()     == 2,  "size is 2 after pop_head");
    check(dq.peak_head()    == 2,  "new head is 2");

    dq.pop_head();
    check(dq.peak_head()    == 3,  "new head is 3");

    dq.pop_head();
    check(dq.is_empty(),           "empty after popping all elements");
}

void test_pop_tail()
{
    section("Pop-tail");
    Circular_dequeue dq(4);
    dq.enqueue_tail(1);
    dq.enqueue_tail(2);
    dq.enqueue_tail(3);

    dq.pop_tail();
    check(dq.get_size()     == 2,  "size is 2 after pop_tail");
    check(dq.peak_tail()    == 2,  "new tail is 2");
}

void test_circular_wraparound()
{
    section("Circular wraparound (key for tiered-vector use)");
    Circular_dequeue dq(4);

    // Fill completely
    dq.enqueue_tail(1);
    dq.enqueue_tail(2);
    dq.enqueue_tail(3);
    dq.enqueue_tail(4);

    // Free two slots from the head, then refill from the tail
    // — this is exactly the pattern a tiered vector uses
    dq.pop_head();
    dq.pop_head();
    dq.enqueue_tail(5);   // should wrap around internally
    dq.enqueue_tail(6);

    check(dq.get_size() == 4, "size == 4 after wraparound refill");
    check(dq[0] == 3,         "[0] == 3");
    check(dq[1] == 4,         "[1] == 4");
    check(dq[2] == 5,         "[2] == 5 (wrapped)");
    check(dq[3] == 6,         "[3] == 6 (wrapped)");
    check(dq.peak_head() == 3,"peak_head == 3");
    check(dq.peak_tail() == 6,"peak_tail == 6");

    // Same idea from the head side
    dq.pop_tail();
    dq.pop_tail();
    dq.enqueue_head(100);  // wraps head pointer backwards
    dq.enqueue_head(200);

    check(dq[0] == 200,       "[0] == 200 (head-wrapped)");
    check(dq[1] == 100,       "[1] == 100 (head-wrapped)");
    check(dq[2] == 3,         "[2] == 3");
    check(dq[3] == 4,         "[3] == 4");
}

void test_interleaved_ops()
{
    section("Interleaved enqueue/pop (stress)");
    Circular_dequeue dq(5);

    for (int i = 1; i <= 5; ++i) dq.enqueue_tail(i * 10);   // 10 20 30 40 50
    dq.pop_head();   // 20 30 40 50
    dq.pop_tail();   // 20 30 40
    dq.enqueue_head(5);   // 5 20 30 40
    dq.enqueue_tail(45);  // 5 20 30 40 45

    check(dq.get_size() == 5,    "size == 5");
    check(dq[0]  == 5,           "[0] == 5");
    check(dq[1]  == 20,          "[1] == 20");
    check(dq[4]  == 45,          "[4] == 45");
    check(dq.is_full(),          "full after interleaved ops");
}

void test_copy_semantics()
{
    section("Copy constructor & copy-assignment");
    Circular_dequeue src(4);
    src.enqueue_tail(1);
    src.enqueue_tail(2);
    src.enqueue_tail(3);

    Circular_dequeue copy_ctor(src);
    check(copy_ctor.get_size() == 3, "copy-ctor: size matches");
    check(copy_ctor[0] == 1,         "copy-ctor: [0] == 1");
    check(copy_ctor[2] == 3,         "copy-ctor: [2] == 3");

    // Mutation of copy must not affect source
    copy_ctor.pop_head();
    check(src.get_size() == 3,       "source unaffected by copy mutation");

    Circular_dequeue copy_assign(4);
    copy_assign = src;
    check(copy_assign.get_size() == 3, "copy-assign: size matches");
    check(copy_assign[1] == 2,         "copy-assign: [1] == 2");

    // Self-assignment
    copy_assign = copy_assign;
    check(copy_assign.get_size() == 3, "self-assignment: size unchanged");
}

void test_insert()
{
    section("Insert");
    Circular_dequeue src(4);
    for (size_t i = 0; i < 3; i++)
    {
        src.insert(i,i+1);
    }
    // [1,2,3]
    src.pop_head();
    src.pop_head();
    // [3]
    src.insert(0,123);
    src.insert(0,6);
    src.insert(3,9);
    
    check(src[0]==6 , "alo");
    check(src[1]==123 , "alo1");
    check(src[2]==3 , "alo2");
    check(src[3]==9 , "alo3");

    check(src.peak_head() == 6, "peah head");
    check(src.peak_tail() == 9 , "peak tail");
}

void test_remove_at()
{
    section("remove");
// --- Single element ---
{
    Circular_dequeue q(4);
    q.insert(0, 100);
    q.remove_at(0);
    check(q.get_size() == 0, "Remove only element → size 0");
    check(q.is_empty(),      "Remove only element → is_empty");
}

// --- Remove head (index 0) ---
{
    Circular_dequeue q(4);
    q.insert(0, 100);
    q.insert(1, 200);
    q.insert(2, 300);
    q.remove_at(0);
    check(q.get_size() == 2,   "Remove head → size 2");
    check(q.peak_head() == 200, "Remove head → new head is 200");
    check(q[0] == 200,         "Remove head → [0] == 200");
    check(q[1] == 300,         "Remove head → [1] == 300");
}

// --- Remove tail (last index) ---
{
    Circular_dequeue q(4);
    q.insert(0, 100);
    q.insert(1, 200);
    q.insert(2, 300);
    q.remove_at(2);
    check(q.get_size() == 2,   "Remove tail → size 2");
    check(q.peak_tail() == 200, "Remove tail → new tail is 200");
    check(q[0] == 100,         "Remove tail → [0] == 100");
    check(q[1] == 200,         "Remove tail → [1] == 200");
}

// --- Remove middle element ---
{
    Circular_dequeue q(4);
    q.insert(0, 10);
    q.insert(1, 20);
    q.insert(2, 30);
    q.insert(3, 40);
    q.remove_at(1);
    check(q.get_size() == 3, "Remove middle → size 3");
    check(q[0] == 10,        "Remove middle → [0] == 10");
    check(q[1] == 30,        "Remove middle → [1] == 30");
    check(q[2] == 40,        "Remove middle → [2] == 40");
}

// --- Remove from circular state (head not at 0) ---
{
    // enqueue_head shifts head backwards, so internal head != 0
    Circular_dequeue q(4);
    q.enqueue_tail(10);
    q.enqueue_tail(20);
    q.enqueue_tail(30);
    q.pop_head();           // head advances by 1 internally
    q.enqueue_tail(40);     // wraps around: [20, 30, 40]
    q.insert(1, 99);        // [20, 99, 30, 40]
    q.remove_at(1);         // [20, 30, 40]
    check(q.get_size() == 3, "Circular state remove middle → size 3");
    check(q[0] == 20,        "Circular state remove middle → [0] == 20");
    check(q[1] == 30,        "Circular state remove middle → [1] == 30");
    check(q[2] == 40,        "Circular state remove middle → [2] == 40");
}

// --- Remove then re-insert ---
{
    Circular_dequeue q(4);
    q.insert(0, 1);
    q.insert(1, 2);
    q.insert(2, 3);
    q.remove_at(1);         // [1, 3]
    q.insert(1, 99);        // [1, 99, 3]
    check(q.get_size() == 3, "Remove then insert → size 3");
    check(q[0] == 1,         "Remove then insert → [0] == 1");
    check(q[1] == 99,        "Remove then insert → [1] == 99");
    check(q[2] == 3,         "Remove then insert → [2] == 3");
}

// --- Out-of-range throws ---
{
    Circular_dequeue q(4);
    q.insert(0, 42);
    bool threw = false;
    try { q.remove_at(1); } catch (const std::out_of_range&) { threw = true; }
    check(threw, "Remove_at out-of-range throws");
}

// --- Remove on empty throws ---
{
    Circular_dequeue q(4);
    bool threw = false;
    try { q.remove_at(0); } catch (const std::out_of_range&) { threw = true; }
    check(threw, "Remove_at on empty dequeue throws");
}
}


void test_move_semantics()
{
    section("Move constructor & move-assignment");
    Circular_dequeue src(4);
    src.enqueue_tail(7);
    src.enqueue_tail(8);

    Circular_dequeue moved(std::move(src));
    check(moved.get_size() == 2,  "move-ctor: size transferred");
    check(moved[0] == 7,          "move-ctor: [0] == 7");
    check(src.is_empty(),         "source empty after move-ctor");

    Circular_dequeue dst(4);
    dst.enqueue_tail(99);
    dst = std::move(moved);
    check(dst.get_size() == 2,    "move-assign: size transferred");
    check(dst[1] == 8,            "move-assign: [1] == 8");
    check(moved.is_empty(),       "source empty after move-assign");
}

// ─── main ────────────────────────────────────────────────────────────────────

int main()
{
    std::cout << "==============================\n";
    std::cout << " Circular_dequeue test suite\n";
    std::cout << "==============================\n";

    test_construction();
    test_enqueue_tail_and_index();
    test_enqueue_head();
    test_full_and_overflow();
    test_pop_head();
    test_pop_tail();
    test_circular_wraparound();
    test_interleaved_ops();
    test_copy_semantics();
    test_insert();
    test_remove_at();
    test_move_semantics();

    std::cout << "\n==============================\n";
    std::cout << " Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "==============================\n";

    return failed == 0 ? 0 : 1;
}