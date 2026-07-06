#pragma once

#include <iostream>
#include <stdexcept>
#include <algorithm>

// Node structure with left_size for number of nodes in left subtree
struct Node 
{
    int   value;
    Node* left;
    Node* right;
    int   height;
    int   left_size;

    Node(int v, Node* l = nullptr, Node* r = nullptr, int h = 0, int ls = 0)
        : value(v), left(l), right(r), height(h), left_size(ls) {}
};

// Indexing AVL tree
class AVLTree 
{
public:
    // Construction

    AVLTree(Node* root = nullptr) : tree_root(root) {}

    AVLTree(const AVLTree& other) 
    {
        tree_root = clone(other.tree_root);
    }

    AVLTree& operator=(const AVLTree& other) 
    {
        if (this != &other) 
        {
            Node* new_root = nullptr;
            try 
            {
                new_root = clone(other.tree_root);
            } catch (...) 
            {
                free_tree(new_root);
                throw;
            }
            free_tree(tree_root);
            tree_root = new_root;
        }
        return *this;
    }

    ~AVLTree() 
    {
        free_tree(tree_root);
    }

    // Functions for the user

    bool contains(int data) const 
    {
        return contains(tree_root, data);
    }

    
    void add(int data) 
    {
        add(tree_root, data);
    }

    void remove(int data) 
    {
        remove(tree_root, data);
    }

    // 0-based index lookup. It returns the value of the nth smallest element. Throws if bigger than the size or negative
    int get_at(int index) const 
    {
        if(tree_root==nullptr)
        {
            throw std::out_of_range("AVL::get_at: tree is empty");
        } 

        int sz = size(tree_root);
        if (index < 0 || index >= sz) throw std::out_of_range("AVLTree::get_at: Index out of range");
        return get_at(tree_root, index);
    }

    // Total number of nodes
    int size() const 
    {
        return size(tree_root);
    }

    // Approximate memory in bytes
    size_t memory_bytes() const
    {
        return sizeof(*this) + size() * sizeof(Node);
    }
    
private:
    // helpers

    static int node_height(const Node* n) 
    {
        return n ? n->height : -1;
    }

    // Total size of a subtree (left size + 1 + right size)
    static int size(const Node* n) 
    {
        if (!n) return 0;
        return n->left_size + 1 + size(n->right);
    }

    // order statistic lookup

    // 0-based index lookup within the subtree rooted at `root`.
    // We assume index is always valid
    static int get_at(const Node* root, int index)
    {        
        // k = number of nodes in the left subtree
        int k = root->left_size;
        
        
        if (index == k)     // root is exactly the kth element
        {
            return root->value;
        } 
        else if (index < k) // answer is somewhere in the left subtree
        {
            return get_at(root->left, index);
        } 
        else                // answer is in the right subtree; adjust index
        {
            return get_at(root->right, index - k - 1);
        }
    }

    // contains 

    static bool contains(const Node* root, int data)
    {
        if (!root)                      return false;
        else if (root->value == data)   return true;
        else if (data < root->value)    return contains(root->left, data);
        else                            return contains(root->right, data);
    }

    // add

    void add(Node*& root, int data)
    {
        if (!root) 
        {
            root = new Node(data);
        } 
        else if (data < root->value)
        {
            add(root->left, data);
            root->left_size++;
        } 
        else if (data > root->value)
        {
            add(root->right, data);
        }
        else balance(root);
    }

    // remove helpers

    static int get_min(const Node* root) 
    {
        if (!root) throw std::invalid_argument("Empty subtree");
        while (root->left) root = root->left;
        return root->value;
    }

    // remove

    void remove(Node*& root, int data) 
    {
        if (!root) return;

        if (data < root->value) 
        {
            remove(root->left, data);
            root->left_size--;
        }
        else if (data > root->value) 
        {
            remove(root->right, data);
        }
        else 
        {
            // Found what type of node to delete
            if (!root->left && !root->right) // Leaf 
            {
                delete root;
                root = nullptr;
                return;
            }
            else if (!root->left) // Only has right child
            {
                Node* to_delete = root;
                root = root->right;
                delete to_delete;
                return;
            }
            else if (!root->right) // Only has left child
            {                
                Node* to_delete = root;
                root = root->left;
                delete to_delete;
                return;
            }
            else 
            // Has two children: replace the root node value with min value in right subtree.
            // Remove from right subtree node with min value
            {
                int min_right = get_min(root->right);
                root->value = min_right;
                remove(root->right, min_right);
            }
        }

        balance(root);
    }

    // rotations

    void right_rotation(Node*& root)
    {
        Node* left_subtree = root->left;

        // left_subtree's right subtree moves to root's left
        root->left = left_subtree->right;

        // root's leftSize shrinks:. It loses left_subtree itself + left_subtree's left subtree
        root->left_size = root->left_size - left_subtree->left_size - 1;

        // left_subtree's right is the old-root
        left_subtree->right = root;

        root->height = std::max(node_height(root->left), node_height(root->right)) + 1;
        left_subtree->height = std::max(node_height(left_subtree->left), root->height) + 1;

        root = left_subtree;
    }

    void left_rotation(Node*& root) 
    {
        Node* right_subtree = root->right;

        // right_subtree's left subtree moves to root's right
        root->right = right_subtree->left;
        
        // right_subtree's left is the old-root
        right_subtree->left = root;

        // right_subtree's leftSize grows: gains root + root's (new) left subtree
        right_subtree->left_size = right_subtree->left_size + root->left_size + 1;

        root->height = std::max(node_height(root->left), node_height(root->right)) + 1;
        right_subtree->height = std::max(node_height(right_subtree->right), root->height) + 1;

        root = right_subtree;
    }

    void left_right_rotation(Node*& root) 
    {
        left_rotation(root->left);
        right_rotation(root);
    }

    void right_left_rotation(Node*& root) 
    {
        right_rotation(root->right);
        left_rotation(root);
    }

    // balance

    void balance(Node*& root) 
    {
        if (!root) return;

        int left_height = node_height(root->left);
        int right_height = node_height(root->right);

        if (left_height - right_height > ALLOWED_IMBALANCE) 
        {
            if (node_height(root->left->left) >= node_height(root->left->right))
                right_rotation(root);
            else
                left_right_rotation(root);
        } else if (right_height - left_height > ALLOWED_IMBALANCE) 
        {
            if (node_height(root->right->right) >= node_height(root->right->left))
                left_rotation(root);
            else
                right_left_rotation(root);
        }

        root->height = std::max(node_height(root->left), node_height(root->right)) + 1;
    }

    // memory

    static void free_tree(Node* root) 
    {
        if (!root) return;
        free_tree(root->left);
        free_tree(root->right);
        delete root;
    }

    static Node* clone(const Node* root) 
    {
        if (!root) return nullptr;
        return new Node(root->value,
                        clone(root->left),
                        clone(root->right),
                        root->height,
                        root->left_size);
    }

private:
    static const int ALLOWED_IMBALANCE = 1;
    Node* tree_root;
};