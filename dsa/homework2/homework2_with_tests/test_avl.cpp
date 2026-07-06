#include <iostream>
#include <string>

// Include your AVL tree implementation here
#include "AVL.hpp"

int passed = 0;
int failed = 0;

void check(bool cond, const std::string& label)
{
    if (cond)
    {
        std::cout << "  [PASS] " << label << '\n';
        ++passed;
    }
    else
    {
        std::cout << "  [FAIL] " << label << '\n';
        ++failed;
    }
}

void section(const std::string& title)
{
    std::cout << "\n=== " << title << " ===\n";
}

void summary()
{
    std::cout << "\n====================================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "====================================\n";
}

bool equals(const AVLTree& tree, const int* arr, int n)
{
    if (tree.size() != n)
        return false;

    for (int i = 0; i < n; i++)
    {
        if (tree.get_at(i) != arr[i])
            return false;
    }

    return true;
}

void print(const AVLTree& tree, const std::string& tag = "")
{
    if (!tag.empty())
        std::cout << "  " << tag << ": [";
    else
        std::cout << "  [";

    for (int i = 0; i < tree.size(); i++)
    {
        std::cout << tree.get_at(i);

        if (i + 1 < tree.size())
            std::cout << ", ";
    }

    std::cout << "] size=" << tree.size() << '\n';
}

// -------------------------------------------------------------

void test_empty_tree()
{
    section("Empty Tree");

    AVLTree tree;

    check(tree.size() == 0, "Initial size is 0");
    check(!tree.contains(5), "Contains returns false");

    bool threw = false;

    try
    {
        tree.get_at(0);
    }
    catch (...)
    {
        threw = true;
    }

    check(threw, "get_at throws on empty tree");
}

void test_single_element()
{
    section("Single Element");

    AVLTree tree;

    tree.add(42);

    check(tree.size() == 1, "Size after insertion");
    check(tree.contains(42), "Contains inserted value");
    check(tree.get_at(0) == 42, "get_at(0)");

    tree.remove(42);

    check(tree.size() == 0, "Size after deletion");
    check(!tree.contains(42), "Value removed");
}

void test_sorted_insertions()
{
    section("Ascending Insertions");

    AVLTree tree;

    for (int i = 1; i <= 20; i++)
        tree.add(i);

    check(tree.size() == 20, "Correct size");

    bool correct = true;

    for (int i = 0; i < 20; i++)
    {
        if (tree.get_at(i) != i + 1)
        {
            correct = false;
            break;
        }
    }

    check(correct, "Elements sorted correctly");
}

void test_reverse_insertions()
{
    section("Descending Insertions");

    AVLTree tree;

    for (int i = 20; i >= 1; i--)
        tree.add(i);

    bool correct = true;

    for (int i = 0; i < 20; i++)
    {
        if (tree.get_at(i) != i + 1)
        {
            correct = false;
            break;
        }
    }

    check(correct, "Elements sorted correctly");
}

void test_duplicates()
{
    section("Duplicate Insertions");

    AVLTree tree;

    tree.add(10);
    tree.add(10);
    tree.add(10);

    int expected[] = {10};

    check(tree.size() == 1, "Duplicates ignored");
    check(equals(tree, expected, 1), "Content correct");
}

void test_contains()
{
    section("Contains");

    AVLTree tree;

    for (int i = 0; i < 100; i++)
        tree.add(i);

    bool foundAll = true;

    for (int i = 0; i < 100; i++)
    {
        if (!tree.contains(i))
        {
            foundAll = false;
            break;
        }
    }

    check(foundAll, "All inserted values found");
    check(!tree.contains(1000), "Missing value not found");
}

void test_remove_leaf()
{
    section("Remove Leaf");

    AVLTree tree;

    tree.add(10);
    tree.add(5);
    tree.add(20);

    tree.remove(5);

    int expected[] = {10, 20};

    check(!tree.contains(5), "Leaf removed");
    check(equals(tree, expected, 2), "Remaining values correct");
}

void test_remove_one_child()
{
    section("Remove Node With One Child");

    AVLTree tree;

    tree.add(10);
    tree.add(5);
    tree.add(2);

    tree.remove(5);

    int expected[] = {2, 10};

    check(!tree.contains(5), "Node removed");
    check(equals(tree, expected, 2), "Tree remains valid");
}

void test_remove_two_children()
{
    section("Remove Node With Two Children");

    AVLTree tree;

    tree.add(50);
    tree.add(30);
    tree.add(70);
    tree.add(20);
    tree.add(40);
    tree.add(60);
    tree.add(80);

    tree.remove(50);

    int expected[] = {20,30,40,60,70,80};

    check(!tree.contains(50), "Node removed");
    check(equals(tree, expected, 6), "Tree remains sorted");
}

void test_remove_root()
{
    section("Remove Root");

    AVLTree tree;

    tree.add(10);
    tree.add(5);
    tree.add(15);

    tree.remove(10);

    int expected[] = {5,15};

    check(equals(tree, expected, 2), "Root removed correctly");
}

void test_copy_constructor()
{
    section("Copy Constructor");

    AVLTree original;

    for (int i = 0; i < 20; i++)
        original.add(i);

    AVLTree copy(original);

    copy.remove(10);

    check(original.contains(10), "Original unchanged");
    check(!copy.contains(10), "Copy modified independently");
}

void test_assignment_operator()
{
    section("Assignment Operator");

    AVLTree first;

    for (int i = 0; i < 20; i++)
        first.add(i);

    AVLTree second;

    second = first;

    second.remove(5);

    check(first.contains(5), "Original unchanged");
    check(!second.contains(5), "Assigned tree independent");
}

void test_self_assignment()
{
    section("Self Assignment");

    AVLTree tree;

    for (int i = 0; i < 50; i++)
        tree.add(i);

    tree = tree;

    bool valid = true;

    for (int i = 0; i < 50; i++)
    {
        if (!tree.contains(i))
        {
            valid = false;
            break;
        }
    }

    check(valid, "Tree survives self-assignment");
}

void test_get_at()
{
    section("get_at");

    AVLTree tree;

    int values[] = {7,2,9,1,5,8,10};

    for (int x : values)
        tree.add(x);

    int expected[] = {1,2,5,7,8,9,10};

    check(equals(tree, expected, 7), "Indexed ordering correct");
}

void test_invalid_indices()
{
    section("Invalid Indices");

    AVLTree tree;

    tree.add(1);

    bool threw1 = false;
    bool threw2 = false;

    try
    {
        tree.get_at(-1);
    }
    catch (...)
    {
        threw1 = true;
    }

    try
    {
        tree.get_at(1);
    }
    catch (...)
    {
        threw2 = true;
    }

    check(threw1, "Negative index throws");
    check(threw2, "Large index throws");
}

void test_large_insertions()
{
    section("Large Insertions");

    AVLTree tree;

    for (int i = 0; i < 5000; i++)
        tree.add(i);

    check(tree.size() == 5000, "5000 elements inserted");
    check(tree.get_at(0) == 0, "First element correct");
    check(tree.get_at(4999) == 4999, "Last element correct");
}

void test_large_removals()
{
    section("Large Removals");

    AVLTree tree;

    for (int i = 0; i < 5000; i++)
        tree.add(i);

    for (int i = 0; i < 5000; i++)
        tree.remove(i);

    check(tree.size() == 0, "All elements removed");
}

void test_sequential_remove()
{
    section("Sequential Remove");

    AVLTree tree;

    for (int i = 0; i < 100; i++)
        tree.add(i);

    bool valid = true;

    for (int i = 0; i < 100; i++)
    {
        tree.remove(i);

        if (tree.contains(i))
        {
            valid = false;
            break;
        }
    }

    check(valid, "Sequential removals work");
    check(tree.size() == 0, "Tree empty");
}

void test_order_after_many_operations()
{
    section("Ordering After Many Operations");

    AVLTree tree;

    tree.add(50);
    tree.add(20);
    tree.add(70);
    tree.add(10);
    tree.add(30);
    tree.add(60);
    tree.add(80);

    tree.remove(20);
    tree.remove(70);

    int expected[] = {10,30,50,60,80};

    check(equals(tree, expected, 5), "Ordering preserved");
}

// -------------------------------------------------------------

int main()
{
    test_empty_tree();
    test_single_element();
    test_sorted_insertions();
    test_reverse_insertions();
    test_duplicates();
    test_contains();
    test_remove_leaf();
    test_remove_one_child();
    test_remove_two_children();
    test_remove_root();
    test_copy_constructor();
    test_assignment_operator();
    test_self_assignment();
    test_get_at();
    test_invalid_indices();
    test_large_insertions();
    test_large_removals();
    test_sequential_remove();
    test_order_after_many_operations();

    summary();

    return failed == 0 ? 0 : 1;
}