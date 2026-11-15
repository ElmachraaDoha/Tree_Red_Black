#ifndef REDBLACKTREE_H_INCLUDED
#define REDBLACKTREE_H_INCLUDED

#include <string>
#include <vector>
#include "Node.h"
#include "Statistics.h"

class RedBlackTree
{
    private:

        Node* root = nullptr;
        int size = 0;               // how many nodes are in the tree
        Statistics stats;           // the counters
        std::string problem;        // what ValidateProperties() found wrong

        // The tree owns its nodes: copying it would delete them twice.
        RedBlackTree(const RedBlackTree&) = delete;
        RedBlackTree& operator=(const RedBlackTree&) = delete;

        void FixInsert(Node* node);
        void FixDelete(Node* node, Node* parent, bool isLeft);

        void RotationL(Node* node);
        void RotationR(Node* node);

        void deleteTree(Node* node);

        Node* minimum(Node* node);
        void transplant(Node* u, Node* v);

        // Small helpers (each counts its own statistics, so nothing is counted twice)
        Node* createNode(int value);
        void freeNode(Node* node);
        void setColor(Node* node, Color color);
        Node* findNode(int value, unsigned long long& comparisons);

        int  height(Node* node);
        void inOrder(Node* node, std::vector<int>& result);
        int  checkNode(Node* node, Node* expectedParent,
                       bool hasLow, int low, bool hasHigh, int high, int& count);

        void Display(Node* node, std::string prefix, bool isLast, char side);

    public:

        RedBlackTree();
        ~RedBlackTree();

        Node* Search(int value);
        void Insert(int value);
        void Delete(int value);
        void Display();

        bool ValidateProperties(bool verbose = false);

        int  Size();
        bool IsEmpty();
        int  Height();
        Node* GetRoot();
        std::vector<int> InOrder();

        const Statistics& GetStatistics() const;
        void ResetStatistics();
        void DisplayStatistics();
};

#endif // REDBLACKTREE_H_INCLUDED
