#include <iostream>
#include <string>
#include "Tiered_vector.hpp"
#include <vector>

int passed = 0;
int failed = 0;

void check(bool cond, const std::string& label) {
    if (cond) { std::cout << "  [PASS] " << label << "\n"; ++passed; }
    else      { std::cout << "  [FAIL] " << label << "\n"; ++failed; }
}

void section(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void print_tv(const Tiered_vector& tv, const std::string& tag = "") {
    if (!tag.empty()) std::cout << "  " << tag << ": [";
    else              std::cout << "  [";

    for (int i = 0; i < tv.size(); ++i) {
        std::cout << tv[i];
        if (i + 1 < tv.size()) std::cout << ", ";
    }

    std::cout << "] (size=" << tv.size()
              << ", cap=" << tv.capacity() << ")\n";
}

bool equals(const Tiered_vector& tv, const int* arr, int n)
{
    if (tv.size() != n)
        return false;

    for (int i = 0; i < n; i++)
        if (tv[i] != arr[i])
            return false;

    return true;
}

int main()
{
    section("Constructor");

    Tiered_vector tv(2); // l = 4

    check(tv.size() == 0, "New vector is empty");
    check(tv.empty(), "Empty() returns true");
    check(tv.capacity() > 0, "Capacity initialized");

    section("Push_back");

    for (int i = 1; i <= 10; i++)
        tv.push_back(i);

    print_tv(tv, "After push_back");

    check(tv.size() == 10, "Size after 10 push_backs");

    bool correct = true;
    for (int i = 0; i < 10; i++)
        correct &= (tv[i] == i + 1);

    check(correct, "Elements stored in correct order");

    section("operator[]");

    check(tv[0] == 1, "First element");
    check(tv[5] == 6, "Middle element");
    check(tv[9] == 10, "Last element");

    section("Insert at beginning");

    tv.insert(0, 100);

    int exp1[] = {100,1,2,3,4,5,6,7,8,9,10};
    print_tv(tv);

    check(equals(tv, exp1, 11), "Insert at rank 0");

    section("Insert in middle");

    tv.insert(5, 200);

    int exp2[] = {100,1,2,3,4,200,5,6,7,8,9,10};

    print_tv(tv);

    check(equals(tv, exp2, 12), "Insert at middle rank");

    section("Insert at end");

    tv.insert(tv.size(), 300);

    int exp3[] = {100,1,2,3,4,200,5,6,7,8,9,10,300};

    print_tv(tv);

    check(equals(tv, exp3, 13), "Insert at end");

    section("Remove beginning");

    tv.remove(0);

    int exp4[] = {1,2,3,4,200,5,6,7,8,9,10,300};

    print_tv(tv);

    check(equals(tv, exp4, 12), "Remove first element");

    section("Remove middle");

    tv.remove(4);

    int exp5[] = {1,2,3,4,5,6,7,8,9,10,300};

    print_tv(tv);

    check(equals(tv, exp5, 11), "Remove middle element");

    section("Remove end");

    tv.remove(tv.size() - 1);

    int exp6[] = {1,2,3,4,5,6,7,8,9,10};

    print_tv(tv);

    check(equals(tv, exp6, 10), "Remove last element");

    section("Pop_back");

    tv.pop_back();
    tv.pop_back();

    int exp7[] = {1,2,3,4,5,6,7,8};

    print_tv(tv);

    check(equals(tv, exp7, 8), "Pop_back removes elements");

    section("Copy constructor");

    Tiered_vector copy(tv);

    check(copy.size() == tv.size(), "Copied size");

    bool same = true;
    for (int i = 0; i < tv.size(); i++)
        same &= (copy[i] == tv[i]);

    check(same, "Copied contents");

    copy.push_back(999);

    check(tv.size() == 8, "Original unaffected by copy modification");
    check(copy.size() == 9, "Copy modified independently");

    section("Copy assignment");

    Tiered_vector assigned(1);
    assigned = tv;

    same = true;
    for (int i = 0; i < tv.size(); i++)
        same &= (assigned[i] == tv[i]);

    check(same, "Assignment copies contents");

    assigned.push_back(777);

    check(tv.size() == 8, "Original unaffected after assignment");

    section("Move constructor");

    Tiered_vector moved(std::move(copy));

    check(moved.size() == 9, "Moved object received data");

    bool moved_ok = true;
    for (int i = 0; i < moved.size() - 1; i++)
        moved_ok &= (moved[i] == tv[i]);

    check(moved_ok, "Move constructor preserved contents");

    section("Move assignment");

    Tiered_vector movedAssign(1);
    movedAssign = std::move(assigned);

    check(movedAssign.size() == 9,
          "Move assignment transferred size");

    section("Large scale push/pop");

    Tiered_vector big(2);

    for (int i = 0; i < 1000; i++)
        big.push_back(i);

    check(big.size() == 1000,
          "1000 push_backs");

    bool large_ok = true;

    for (int i = 0; i < 1000; i++)
    {
        if (big[i] != i)
        {
            large_ok = false;
            break;
        }
    }

    check(large_ok,
          "Large sequence integrity");

    for (int i = 0; i < 500; i++)
        big.pop_back();

    check(big.size() == 500,
          "500 pop_backs");


    section("Insert every position");

{
    Tiered_vector tv(2);

    tv.push_back(1);
    tv.push_back(2);
    tv.push_back(3);
    tv.push_back(4);

    for (int pos = 0; pos <= 4; pos++)
    {
        Tiered_vector temp(tv);

        temp.insert(pos, 99);

        check(temp.size() == 5,
              "Insert at position " + std::to_string(pos));

        check(temp[pos] == 99,
              "Inserted value appears at correct rank "
              + std::to_string(pos));
    }
}

section("Remove every position");

{
    for (int pos = 0; pos < 5; pos++)
    {
        Tiered_vector tv(2);

        for (int i = 0; i < 5; i++)
            tv.push_back(i);

        tv.remove(pos);

        check(tv.size() == 4,
              "Remove at position " + std::to_string(pos));

        bool found = false;

        for (int i = 0; i < tv.size(); i++)
            if (tv[i] == pos)
                found = true;

        check(!found,
              "Removed value disappeared");
    }
}

section("Repeated front inserts");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 100; i++)
        tv.insert(0, i);

    check(tv.size() == 100,
          "100 front inserts");

    check(tv[0] == 99,
          "Newest front element");

    check(tv[99] == 0,
          "Oldest element moved to back");
}



section("Repeated front removes");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 100; i++)
        tv.push_back(i);

    for (int i = 0; i < 50; i++)
        tv.remove(0);

    check(tv.size() == 50,
          "50 front removals");

    check(tv[0] == 50,
          "Front removal shifts correctly");
}




section("Alternate insert/remove");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 100; i++)
    {
        tv.push_back(i);
        tv.remove(tv.size() - 1);
    }

    check(tv.empty(),
          "Alternating push/remove keeps structure valid");
}




section("Expansion test");

{
    Tiered_vector tv(1);

    int oldCap = tv.capacity();

    while (tv.capacity() == oldCap)
        tv.push_back(tv.size());

    check(tv.capacity() > oldCap,
          "Capacity increased");

    bool ok = true;

    for (int i = 0; i < tv.size(); i++)
        ok &= (tv[i] == i);

    check(ok,
          "Expansion preserved order");
}



section("Multiple expansions");

{
    Tiered_vector tv(1);

    for (int i = 0; i < 5000; i++)
        tv.push_back(i);

    bool ok = true;

    for (int i = 0; i < 5000; i++)
    {
        if (tv[i] != i)
        {
            ok = false;
            break;
        }
    }

    check(ok,
          "Data correct after many expansions");
}




section("Shrink test");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 1000; i++)
        tv.push_back(i);

    int peakCap = tv.capacity();

    for (int i = 0; i < 950; i++)
        tv.pop_back();

    check(tv.size() == 50,
          "Elements removed");

    check(tv.capacity() <= peakCap,
          "Shrink possibly occurred");
}


section("Deep copy validation");

{
    Tiered_vector a(2);

    for (int i = 0; i < 50; i++)
        a.push_back(i);

    Tiered_vector b(a);

    for (int i = 0; i < 50; i++)
        b.remove(0);

    check(a.size() == 50,
          "Original size unchanged");

    check(b.size() == 0,
          "Copy modified independently");
}



section("Self assignment");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 20; i++)
        tv.push_back(i);

    tv = tv;

    bool ok = true;

    for (int i = 0; i < 20; i++)
        ok &= (tv[i] == i);

    check(ok,
          "Self assignment preserved data");
}


section("Move self assignment");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 20; i++)
        tv.push_back(i);

    tv = std::move(tv);

    check(tv.size() >= 0,
          "Move self assignment didn't crash");
}


section("Tier boundary inserts");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 16; i++)
        tv.push_back(i);

    tv.insert(4, 111);
    tv.insert(9, 222);
    tv.insert(14, 333);

    check(tv[4] == 111,
          "Insert exactly at tier boundary");

    check(tv[9] == 222,
          "Insert across tier boundary");

    check(tv[14] == 333,
          "Insert near boundary");
}


section("Tier boundary removes");

{
    Tiered_vector tv(2);

    for (int i = 0; i < 20; i++)
        tv.push_back(i);

    tv.remove(4);
    tv.remove(7);
    tv.remove(10);

    check(tv.size() == 17,
          "Boundary removals size correct");
}



// section("Randomized stress");

// {
//     Tiered_vector tv(2);
//     std::vector<int> ref;

//     for (int step = 0; step < 10000; step++)
//     {
//         int op = rand() % 4;

//         if (op == 0 || ref.empty())
//         {
//             int v = rand();

//             tv.Push_back(v);
//             ref.push_back(v);
//         }
//         else if (op == 1)
//         {
//             tv.Pop_back();
//             ref.pop_back();
//         }
//         else if (op == 2)
//         {
//             int pos = rand() % (ref.size() + 1);
//             int v = rand();

//             tv.Insert(pos, v);
//             ref.insert(ref.begin() + pos, v);
//         }
//         else
//         {
//             int pos = rand() % ref.size();

//             tv.Remove(pos);
//             ref.erase(ref.begin() + pos);
//         }

//         bool ok = tv.Size() == ref.size();

//         for (int i = 0; ok && i < tv.Size(); i++)
//             ok &= (tv[i] == ref[i]);

//         if (!ok)
//         {
//             check(false,
//                   "Randomized stress test");

//             break;
//         }

//         if (step == 9999)
//             check(true,
//                   "Randomized stress test");
//     }
// }

    section("Summary");

    std::cout << "\nPassed: " << passed
              << "\nFailed: " << failed
              << "\n";

    return failed == 0 ? 0 : 1;
}


/*
#include <iostream>
#include <string>
#include "Tiered_vector.hpp"

int passed = 0;
int failed = 0;

void check(bool cond, const std::string& label) {
    if (cond) { std::cout << "  [PASS] " << label << "\n"; ++passed; }
    else      { std::cout << "  [FAIL] " << label << "\n"; ++failed; }
}

void section(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void print_tv(const Tiered_vector& tv, const std::string& tag = "") {
    if (!tag.empty()) std::cout << "  " << tag << ": [";
    else              std::cout << "  [";

    for (int i = 0; i < tv.Size(); ++i) {
        std::cout << tv[i];
        if (i + 1 < tv.Size()) std::cout << ", ";
    }

    std::cout << "] (size=" << tv.Size()
              << ", cap=" << tv.Capacity() << ")\n";
}

bool equals(const Tiered_vector& tv, const int* arr, int n)
{
    if (tv.Size() != n)
        return false;

    for (int i = 0; i < n; i++)
        if (tv[i] != arr[i])
            return false;

    return true;
}

int main()
{
    section("Constructor");

    Tiered_vector tv(2); // l = 4

    check(tv.Size() == 0, "New vector is empty");
    check(tv.Empty(), "Empty() returns true");
    check(tv.Capacity() > 0, "Capacity initialized");

    section("Push_back");

    for (int i = 1; i <= 10; i++)
        tv.Push_back(i);

    print_tv(tv, "After push_back");

    check(tv.Size() == 10, "Size after 10 push_backs");

    bool correct = true;
    for (int i = 0; i < 10; i++)
        correct &= (tv[i] == i + 1);

    check(correct, "Elements stored in correct order");

    section("operator[]");

    check(tv[0] == 1, "First element");
    check(tv[5] == 6, "Middle element");
    check(tv[9] == 10, "Last element");

    section("Insert at beginning");

    tv.Insert(0, 100);

    int exp1[] = {100,1,2,3,4,5,6,7,8,9,10};
    print_tv(tv);

    check(equals(tv, exp1, 11), "Insert at rank 0");

    section("Insert in middle");

    tv.Insert(5, 200);

    int exp2[] = {100,1,2,3,4,200,5,6,7,8,9,10};

    print_tv(tv);

    check(equals(tv, exp2, 12), "Insert at middle rank");

    section("Insert at end");

    tv.Insert(tv.Size(), 300);

    int exp3[] = {100,1,2,3,4,200,5,6,7,8,9,10,300};

    print_tv(tv);

    check(equals(tv, exp3, 13), "Insert at end");

    section("Remove beginning");

    tv.Remove(0);

    int exp4[] = {1,2,3,4,200,5,6,7,8,9,10,300};

    print_tv(tv);

    check(equals(tv, exp4, 12), "Remove first element");

    section("Remove middle");

    tv.Remove(4);

    int exp5[] = {1,2,3,4,5,6,7,8,9,10,300};

    print_tv(tv);

    check(equals(tv, exp5, 11), "Remove middle element");

    section("Remove end");

    tv.Remove(tv.Size() - 1);

    int exp6[] = {1,2,3,4,5,6,7,8,9,10};

    print_tv(tv);

    check(equals(tv, exp6, 10), "Remove last element");

    section("Pop_back");

    tv.Pop_back();
    tv.Pop_back();

    int exp7[] = {1,2,3,4,5,6,7,8};

    print_tv(tv);

    check(equals(tv, exp7, 8), "Pop_back removes elements");

    section("Copy constructor");

    Tiered_vector copy(tv);

    check(copy.Size() == tv.Size(), "Copied size");

    bool same = true;
    for (int i = 0; i < tv.Size(); i++)
        same &= (copy[i] == tv[i]);

    check(same, "Copied contents");

    copy.Push_back(999);

    check(tv.Size() == 8, "Original unaffected by copy modification");
    check(copy.Size() == 9, "Copy modified independently");

    section("Copy assignment");

    Tiered_vector assigned(1);
    assigned = tv;

    same = true;
    for (int i = 0; i < tv.Size(); i++)
        same &= (assigned[i] == tv[i]);

    check(same, "Assignment copies contents");

    assigned.Push_back(777);

    check(tv.Size() == 8, "Original unaffected after assignment");

    section("Move constructor");

    Tiered_vector moved(std::move(copy));

    check(moved.Size() == 9, "Moved object received data");

    bool moved_ok = true;
    for (int i = 0; i < moved.Size() - 1; i++)
        moved_ok &= (moved[i] == tv[i]);

    check(moved_ok, "Move constructor preserved contents");

    section("Move assignment");

    Tiered_vector movedAssign(1);
    movedAssign = std::move(assigned);

    check(movedAssign.Size() == 9,
          "Move assignment transferred size");

    section("Large scale push/pop");

    Tiered_vector big(2);

    for (int i = 0; i < 1000; i++)
        big.Push_back(i);

    check(big.Size() == 1000,
          "1000 push_backs");

    bool large_ok = true;

    for (int i = 0; i < 1000; i++)
    {
        if (big[i] != i)
        {
            large_ok = false;
            break;
        }
    }

    check(large_ok,
          "Large sequence integrity");

    for (int i = 0; i < 500; i++)
        big.Pop_back();

    check(big.Size() == 500,
          "500 pop_backs");

    section("Summary");

    std::cout << "\nPassed: " << passed
              << "\nFailed: " << failed
              << "\n";

    return failed == 0 ? 0 : 1;
}
*/
