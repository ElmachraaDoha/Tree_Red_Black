#include "RedBlackTree.h"
#include <iostream>

using namespace std;

// =====================================================================
//  Constructor / Destructor
// =====================================================================

RedBlackTree::RedBlackTree()
{
    root = nullptr;
    size = 0;
}

RedBlackTree::~RedBlackTree()
{
    deleteTree(root);
}

void RedBlackTree::deleteTree(Node* node)
{
    if (node == nullptr)
        return;

    deleteTree(node->left);
    deleteTree(node->right);

    freeNode(node);
}

// =====================================================================
//  Small helpers (memory + statistics are handled in ONE place each,
//  so nothing can be counted twice)
// =====================================================================

Node* RedBlackTree::createNode(int value)
{
    Node* node = new Node;

    node->value = value;
    node->color = RED;          // a new node is always RED
    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;

    stats.allocations++;
    stats.currentNodes++;

    if (stats.currentNodes > stats.peakNodes)
        stats.peakNodes = stats.currentNodes;

    return node;
}

void RedBlackTree::freeNode(Node* node)
{
    delete node;

    stats.deallocations++;
    stats.currentNodes--;
}

// Every color change goes through this function.
// It counts a recoloring ONLY if the color really changes.
void RedBlackTree::setColor(Node* node, Color color)
{
    if (node->color != color)
    {
        node->color = color;
        stats.recolorings++;
    }
}

// Search loop shared by Search() and Delete().
// The caller says which counter must be increased, so a Delete
// is never counted as a Search.
Node* RedBlackTree::findNode(int value, unsigned long long& comparisons)
{
    Node* crt = root;

    while (crt != nullptr)
    {
        comparisons++;                  // one comparison per visited node

        if (value == crt->value)
            return crt;

        if (value < crt->value)
            crt = crt->left;
        else
            crt = crt->right;
    }

    return nullptr;
}

// =====================================================================
//  Display
// =====================================================================

void RedBlackTree::Display()
{
    if (root == nullptr)
    {
        cout << "(empty tree)" << endl;
        return;
    }

    if (size > 100)
    {
        cout << "(tree has " << size << " nodes: too big to display, limit is 100)" << endl;
        return;
    }

    // The root is printed first, then its children with branches.
    cout << root->value << " ("
         << (root->color == RED ? "RED" : "BLACK") << ") [ROOT]" << endl;

    bool hasLeft  = (root->left  != nullptr);
    bool hasRight = (root->right != nullptr);

    if (hasLeft)
        Display(root->left, "", !hasRight, 'L');
    if (hasRight)
        Display(root->right, "", true, 'R');

    cout << "(L = left child, R = right child, a missing side is NIL = BLACK)" << endl;
}

// prefix : the "|   " and "    " pieces drawn by the ancestors
// isLast : true if this node is the last child of its parent
// side   : 'L' or 'R'
void RedBlackTree::Display(Node* node, string prefix, bool isLast, char side)
{
    cout << prefix << (isLast ? "`-- " : "|-- ")
         << side << ": " << node->value
         << " (" << (node->color == RED ? "RED" : "BLACK") << ")" << endl;

    string childPrefix = prefix + (isLast ? "    " : "|   ");

    bool hasLeft  = (node->left  != nullptr);
    bool hasRight = (node->right != nullptr);

    if (hasLeft)
        Display(node->left, childPrefix, !hasRight, 'L');
    if (hasRight)
        Display(node->right, childPrefix, true, 'R');
}

// =====================================================================
//  Search / Insert
// =====================================================================

Node* RedBlackTree::Search(int value)
{
    return findNode(value, stats.searchComparisons);
}

void RedBlackTree::Insert(int value)
{
    Node* parent = nullptr;
    Node* current = root;

    // Find the correct position
    while (current != nullptr)
    {
        parent = current;
        stats.insertComparisons++;

        if (value < current->value)
            current = current->left;
        else if (value > current->value)
            current = current->right;
        else
            return;     // duplicate: ignored (nothing was allocated yet)
    }

    // Only now we know the value is new, so we allocate the node.
    Node* newNode = createNode(value);
    newNode->parent = parent;

    // Insert the node
    if (parent == nullptr)
        root = newNode;
    else if (value < parent->value)
        parent->left = newNode;
    else
        parent->right = newNode;

    size++;

    // Restore Red-Black properties
    FixInsert(newNode);
}

Node* RedBlackTree::minimum(Node* node)
{
    while (node->left != nullptr)
        node = node->left;

    return node;
}

void RedBlackTree::transplant(Node* u, Node* v)
{
    if (u->parent == nullptr)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    if (v != nullptr)
        v->parent = u->parent;
}

// =====================================================================
//  Insertion balancing
// =====================================================================

void RedBlackTree::FixInsert(Node* node)
{
    while (node != root && node->parent->color == RED)
    {
        Node* parent = node->parent;
        Node* grandparent = parent->parent;

        // Parent is on the left
        if (parent == grandparent->left)
        {
            Node* uncle = grandparent->right;

            // Case 1: Uncle is RED
            if (uncle != nullptr && uncle->color == RED)
            {
                setColor(parent, BLACK);
                setColor(uncle, BLACK);
                setColor(grandparent, RED);

                node = grandparent;
            }
            else
            {
                // Case 2: Triangle
                if (node == parent->right)
                {
                    node = parent;
                    RotationL(node);

                    parent = node->parent;
                    grandparent = parent->parent;
                }

                // Case 3: Line
                setColor(parent, BLACK);
                setColor(grandparent, RED);

                RotationR(grandparent);
            }
        }
        else
        {
            Node* uncle = grandparent->left;

            // Case 1: Uncle is RED
            if (uncle != nullptr && uncle->color == RED)
            {
                setColor(parent, BLACK);
                setColor(uncle, BLACK);
                setColor(grandparent, RED);

                node = grandparent;
            }
            else
            {
                // Case 2: Triangle
                if (node == parent->left)
                {
                    node = parent;
                    RotationR(node);

                    parent = node->parent;
                    grandparent = parent->parent;
                }

                // Case 3: Line
                setColor(parent, BLACK);
                setColor(grandparent, RED);

                RotationL(grandparent);
            }
        }
    }

    setColor(root, BLACK);
}

// =====================================================================
//  Rotations
// =====================================================================

void RedBlackTree::RotationL(Node* node)
{
    stats.leftRotations++;

    Node* y = node->right;

    node->right = y->left;

    if (y->left != nullptr)
        y->left->parent = node;

    y->parent = node->parent;

    if (node->parent == nullptr)
        root = y;
    else if (node == node->parent->left)
        node->parent->left = y;
    else
        node->parent->right = y;

    y->left = node;
    node->parent = y;
}

void RedBlackTree::RotationR(Node* node)
{
    stats.rightRotations++;

    Node* y = node->left;

    node->left = y->right;

    if (y->right != nullptr)
        y->right->parent = node;

    y->parent = node->parent;

    if (node->parent == nullptr)
        root = y;
    else if (node == node->parent->right)
        node->parent->right = y;
    else
        node->parent->left = y;

    y->right = node;
    node->parent = y;
}

// =====================================================================
//  Deletion balancing
// =====================================================================

void RedBlackTree::FixDelete(Node* node, Node* parent, bool isLeft)
{
    while (node != root && (node == nullptr || node->color == BLACK))
    {
        if (parent == nullptr)
            break;

        bool leftSide;

        if (node != nullptr)
            leftSide = (node == parent->left);
        else
            leftSide = isLeft;

        // Case: node is on the left
        if (leftSide)
        {
            Node* sibling = parent->right;

            // Case 1: Sibling is RED
            if (sibling != nullptr && sibling->color == RED)
            {
                setColor(sibling, BLACK);
                setColor(parent, RED);

                RotationL(parent);

                sibling = parent->right;
            }

            // Case 2: Sibling is BLACK with two BLACK children
            if (sibling == nullptr ||
                ((sibling->left == nullptr || sibling->left->color == BLACK) &&
                 (sibling->right == nullptr || sibling->right->color == BLACK)))
            {
                if (sibling != nullptr)
                    setColor(sibling, RED);

                node = parent;
                parent = node->parent;

                isLeft = (parent != nullptr && node == parent->left);
            }
            else
            {
                // Case 3: Far child is BLACK
                if (sibling->right == nullptr || sibling->right->color == BLACK)
                {
                    if (sibling->left != nullptr)
                        setColor(sibling->left, BLACK);

                    setColor(sibling, RED);

                    RotationR(sibling);

                    sibling = parent->right;
                }

                // Case 4: Far child is RED
                setColor(sibling, parent->color);
                setColor(parent, BLACK);

                if (sibling->right != nullptr)
                    setColor(sibling->right, BLACK);

                RotationL(parent);

                node = root;
                parent = nullptr;
            }
        }
        else
        {
            // Symmetrical cases: node is on the right
            Node* sibling = parent->left;

            // Case 1: Sibling is RED
            if (sibling != nullptr && sibling->color == RED)
            {
                setColor(sibling, BLACK);
                setColor(parent, RED);

                RotationR(parent);

                sibling = parent->left;
            }

            // Case 2: Sibling is BLACK with two BLACK children
            if (sibling == nullptr ||
                ((sibling->left == nullptr || sibling->left->color == BLACK) &&
                 (sibling->right == nullptr || sibling->right->color == BLACK)))
            {
                if (sibling != nullptr)
                    setColor(sibling, RED);

                node = parent;
                parent = node->parent;

                isLeft = (parent != nullptr && node == parent->left);
            }
            else
            {
                // Case 3: Far child is BLACK
                if (sibling->left == nullptr || sibling->left->color == BLACK)
                {
                    if (sibling->right != nullptr)
                        setColor(sibling->right, BLACK);

                    setColor(sibling, RED);

                    RotationL(sibling);

                    sibling = parent->left;
                }

                // Case 4: Far child is RED
                setColor(sibling, parent->color);
                setColor(parent, BLACK);

                if (sibling->left != nullptr)
                    setColor(sibling->left, BLACK);

                RotationR(parent);

                node = root;
                parent = nullptr;
            }
        }
    }

    if (node != nullptr)
        setColor(node, BLACK);
}

// =====================================================================
//  Delete
// =====================================================================

void RedBlackTree::Delete(int value)
{
    Node* z = findNode(value, stats.deleteComparisons);

    if (z == nullptr)
        return;

    Node* y = z;
    Node* x = nullptr;
    Node* xParent = nullptr;

    bool isLeft = false;

    Color originalColor = y->color;

    // Case 1: No left child
    if (z->left == nullptr)
    {
        x = z->right;
        xParent = z->parent;

        isLeft = (xParent != nullptr && z == xParent->left);

        transplant(z, z->right);

        if (x != nullptr)
        {
            xParent = x->parent;
            isLeft = (xParent != nullptr && x == xParent->left);
        }
    }

    // Case 2: No right child
    else if (z->right == nullptr)
    {
        x = z->left;
        xParent = z->parent;

        isLeft = (xParent != nullptr && z == xParent->left);

        transplant(z, z->left);

        if (x != nullptr)
        {
            xParent = x->parent;
            isLeft = (xParent != nullptr && x == xParent->left);
        }
    }

    // Case 3: Two children
    else
    {
        y = minimum(z->right);
        originalColor = y->color;

        x = y->right;

        if (y->parent == z)
        {
            xParent = y;
            isLeft = false;
        }
        else
        {
            xParent = y->parent;
            isLeft = (y == xParent->left);

            transplant(y, y->right);

            y->right = z->right;
            y->right->parent = y;
        }

        transplant(z, y);

        y->left = z->left;
        y->left->parent = y;

        setColor(y, z->color);      // y takes the color of the deleted node

        if (x != nullptr)
        {
            xParent = x->parent;
            isLeft = (xParent != nullptr && x == xParent->left);
        }
    }

    freeNode(z);
    size--;

    // Fix the tree if the removed node was BLACK
    if (originalColor == BLACK)
        FixDelete(x, xParent, isLeft);
}

// =====================================================================
//  Validation of the 5 Red-Black properties
// =====================================================================

// Checks one node and everything below it.
// Returns the black height of this subtree, or -1 if a rule is broken
// (the reason is saved in "problem").
int RedBlackTree::checkNode(Node* node, Node* expectedParent,
                            bool hasLow, int low, bool hasHigh, int high, int& count)
{
    // Property 3: a NIL leaf is BLACK, so it counts as 1 black node.
    if (node == nullptr)
        return 1;

    // Safety: more nodes than "size" means the links are broken.
    count++;
    if (count > size)
    {
        problem = "more nodes found than expected (broken links)";
        return -1;
    }

    // Property 1: every node is RED or BLACK
    if (node->color != RED && node->color != BLACK)
    {
        problem = "property 1: node " + std::to_string(node->value) + " has no valid color";
        return -1;
    }

    // The parent pointer must point back to the node we came from
    if (node->parent != expectedParent)
    {
        problem = "node " + std::to_string(node->value) + " has a wrong parent pointer";
        return -1;
    }

    // Binary search tree order: low < value < high
    if ((hasLow && node->value <= low) || (hasHigh && node->value >= high))
    {
        problem = "binary search tree order broken at node " + std::to_string(node->value);
        return -1;
    }

    // Property 4: a RED node cannot have a RED child
    if (node->color == RED)
    {
        if ((node->left != nullptr && node->left->color == RED) ||
            (node->right != nullptr && node->right->color == RED))
        {
            problem = "property 4: RED node " + std::to_string(node->value) + " has a RED child";
            return -1;
        }
    }

    int leftHeight = checkNode(node->left, node, hasLow, low, true, node->value, count);
    if (leftHeight == -1)
        return -1;

    int rightHeight = checkNode(node->right, node, true, node->value, hasHigh, high, count);
    if (rightHeight == -1)
        return -1;

    // Property 5: same number of BLACK nodes on the left and on the right
    if (leftHeight != rightHeight)
    {
        problem = "property 5: different black heights under node " + std::to_string(node->value);
        return -1;
    }

    return leftHeight + (node->color == BLACK ? 1 : 0);
}

bool RedBlackTree::ValidateProperties(bool verbose)
{
    problem = "";
    int count = 0;
    int blackHeight = 1;
    bool valid = true;

    // Property 2: the root is BLACK
    if (root != nullptr && root->color != BLACK)
    {
        problem = "property 2: the root is not BLACK";
        valid = false;
    }
    else if (root != nullptr && root->parent != nullptr)
    {
        problem = "the root has a parent";
        valid = false;
    }

    if (valid)
    {
        blackHeight = checkNode(root, nullptr, false, 0, false, 0, count);

        if (blackHeight == -1)
            valid = false;
        else if (count != size)
        {
            problem = "the number of nodes found differs from the size";
            valid = false;
        }
    }

    if (verbose)
    {
        if (valid)
        {
            cout << "Red-Black check: VALID" << endl;
            cout << "  1. every node is RED or BLACK ........ OK" << endl;
            cout << "  2. the root is BLACK ................. OK" << endl;
            cout << "  3. NIL leaves are BLACK .............. OK (nullptr = BLACK)" << endl;
            cout << "  4. no RED node has a RED child ....... OK" << endl;
            cout << "  5. same black height on every path ... OK (black height = "
                 << blackHeight << ")" << endl;
        }
        else
        {
            cout << "Red-Black check: INVALID -> " << problem << endl;
        }
    }

    return valid;
}

// =====================================================================
//  Information
// =====================================================================

int RedBlackTree::Size()
{
    return size;
}

bool RedBlackTree::IsEmpty()
{
    return root == nullptr;
}

Node* RedBlackTree::GetRoot()
{
    return root;
}

int RedBlackTree::height(Node* node)
{
    if (node == nullptr)
        return 0;

    int l = height(node->left);
    int r = height(node->right);

    return (l > r ? l : r) + 1;
}

int RedBlackTree::Height()
{
    return height(root);
}

void RedBlackTree::inOrder(Node* node, vector<int>& result)
{
    if (node == nullptr)
        return;

    inOrder(node->left, result);
    result.push_back(node->value);
    inOrder(node->right, result);
}

vector<int> RedBlackTree::InOrder()
{
    vector<int> result;
    inOrder(root, result);
    return result;
}

// =====================================================================
//  Statistics
// =====================================================================

const Statistics& RedBlackTree::GetStatistics() const
{
    return stats;
}

// Operation counters go back to 0. The nodes that are still in the tree
// are really allocated, so "current" keeps its true value and "peak"
// restarts from it.
void RedBlackTree::ResetStatistics()
{
    unsigned long long alive = stats.currentNodes;

    stats.Reset();

    stats.currentNodes = alive;
    stats.peakNodes = alive;
}

void RedBlackTree::DisplayStatistics()
{
    stats.Display();
}
