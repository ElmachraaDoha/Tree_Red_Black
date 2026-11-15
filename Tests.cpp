#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Tests.h"
#include "RedBlackTree.h"
#include "MapContainer.h"

using namespace std;

// =====================================================================
//  How the tests work
//  - Every test does the SAME operations on our tree and on the map.
//  - After the operations we check:
//      * the tree respects the 5 Red-Black rules (ValidateProperties)
//      * the tree and the map have the same size and the same values
//  - A test is PASS if nothing went wrong, FAIL otherwise.
// =====================================================================

static bool testOk = true;
static int passed = 0;
static int failed = 0;

// Prints a message and marks the test as failed if "condition" is false.
static void expect(bool condition, const string& message)
{
    if (!condition)
    {
        testOk = false;
        cout << "      [!] " << message << endl;
    }
}

static void runTest(int number, const string& name, void (*test)())
{
    testOk = true;

    cout << "Test " << number << ": " << name << endl;
    test();

    if (testOk)
    {
        cout << "  [PASS]" << endl << endl;
        passed++;
    }
    else
    {
        cout << "  [FAIL]" << endl << endl;
        failed++;
    }
}

// ---- Helpers: do one operation on both structures ----

static void addBoth(RedBlackTree& tree, MapContainer& map, int key)
{
    tree.Insert(key);
    map.Insert(key);
}

static void removeBoth(RedBlackTree& tree, MapContainer& map, int key)
{
    tree.Delete(key);
    map.Delete(key);
}

static void fillSorted(RedBlackTree& tree, MapContainer& map, int n)
{
    for (int i = 1; i <= n; i++)
        addBoth(tree, map, i);
}

// Checks the tree (5 rules) and compares it with the map.
static void checkSame(RedBlackTree& tree, MapContainer& map, const string& when)
{
    if (!tree.ValidateProperties())
        expect(false, "the tree breaks the Red-Black rules " + when);

    expect(tree.Size() == map.Size(), "tree and map have different sizes " + when);
    expect(tree.InOrder() == map.Keys(), "tree and map hold different values " + when);
}

// Checks that the tree and the map give the expected answer for a search.
static void checkSearch(RedBlackTree& tree, MapContainer& map, int key, bool shouldExist)
{
    expect((tree.Search(key) != nullptr) == shouldExist,
           "tree: search(" + to_string(key) + ") gave the wrong answer");
    expect(map.Search(key) == shouldExist,
           "map: search(" + to_string(key) + ") gave the wrong answer");
}

// =====================================================================
//  The 14 required tests
// =====================================================================

// 1
static void testEmptyTree()
{
    RedBlackTree tree;
    MapContainer map;

    expect(tree.IsEmpty(), "a new tree must be empty");
    expect(map.Size() == 0, "a new map must be empty");
    checkSearch(tree, map, 42, false);
    checkSame(tree, map, "on empty structures");

    tree.Display();
}

// 2
static void testInsertOne()
{
    RedBlackTree tree;
    MapContainer map;

    addBoth(tree, map, 10);

    expect(tree.Size() == 1, "size must be 1");
    checkSearch(tree, map, 10, true);
    checkSame(tree, map, "after inserting one element");

    Node* root = tree.GetRoot();
    expect(root != nullptr && root->color == BLACK, "the root must be BLACK");

    tree.Display();
}

// 3
static void testInsertMultiple()
{
    RedBlackTree tree;
    MapContainer map;
    int values[] = {10, 20, 30, 15, 25, 5};

    for (int i = 0; i < 6; i++)
    {
        addBoth(tree, map, values[i]);
        checkSame(tree, map, "after inserting " + to_string(values[i]));
    }

    int sorted[] = {5, 10, 15, 20, 25, 30};
    expect(tree.InOrder() == vector<int>(sorted, sorted + 6), "values must come out sorted");

    tree.Display();
}

// 4
static void testSearchExisting()
{
    RedBlackTree tree;
    MapContainer map;
    fillSorted(tree, map, 10);

    for (int i = 1; i <= 10; i++)
        checkSearch(tree, map, i, true);

    checkSame(tree, map, "after searching (a search must not change anything)");
}

// 5
static void testSearchNonExisting()
{
    RedBlackTree tree;
    MapContainer map;
    fillSorted(tree, map, 10);

    checkSearch(tree, map, 0, false);
    checkSearch(tree, map, 11, false);
    checkSearch(tree, map, -5, false);
    checkSearch(tree, map, 1000, false);
}

// 6
static void testDeleteLeaf()
{
    // (a) a RED leaf: in the tree 1..7 the leaf 7 is RED
    {
        RedBlackTree tree;
        MapContainer map;
        fillSorted(tree, map, 7);
        cout << "  Before deleting the RED leaf 7:" << endl;
        tree.Display();

        Node* leaf = tree.Search(7);
        expect(leaf != nullptr && leaf->left == nullptr && leaf->right == nullptr && leaf->color == RED,
               "7 must be a RED leaf");

        removeBoth(tree, map, 7);
        checkSearch(tree, map, 7, false);
        checkSame(tree, map, "after deleting a RED leaf");
    }

    // (b) a BLACK leaf: in the tree 10,5,15,3 the leaf 15 is BLACK
    {
        RedBlackTree tree;
        MapContainer map;
        addBoth(tree, map, 10);
        addBoth(tree, map, 5);
        addBoth(tree, map, 15);
        addBoth(tree, map, 3);
        cout << "  Before deleting the BLACK leaf 15:" << endl;
        tree.Display();

        Node* leaf = tree.Search(15);
        expect(leaf != nullptr && leaf->left == nullptr && leaf->right == nullptr && leaf->color == BLACK,
               "15 must be a BLACK leaf");

        removeBoth(tree, map, 15);
        checkSearch(tree, map, 15, false);
        checkSame(tree, map, "after deleting a BLACK leaf");

        cout << "  After:" << endl;
        tree.Display();
    }
}

// 7
static void testDeleteOneChild()
{
    // In the tree 10,5,15,3 the node 5 has only one child (3).
    RedBlackTree tree;
    MapContainer map;
    addBoth(tree, map, 10);
    addBoth(tree, map, 5);
    addBoth(tree, map, 15);
    addBoth(tree, map, 3);

    Node* n = tree.Search(5);
    expect(n != nullptr && n->left != nullptr && n->right == nullptr, "5 must have exactly one child");

    removeBoth(tree, map, 5);

    checkSearch(tree, map, 5, false);
    checkSearch(tree, map, 3, true);
    checkSame(tree, map, "after deleting a node with one child");

    cout << "  After deleting 5:" << endl;
    tree.Display();
}

// 8
static void testDeleteTwoChildren()
{
    RedBlackTree tree;
    MapContainer map;
    fillSorted(tree, map, 7);

    cout << "  Before deleting 2 and 6 (two children each):" << endl;
    tree.Display();

    Node* n = tree.Search(2);
    expect(n != nullptr && n->left != nullptr && n->right != nullptr, "2 must have two children");
    removeBoth(tree, map, 2);
    checkSame(tree, map, "after deleting 2");

    n = tree.Search(6);
    expect(n != nullptr && n->left != nullptr && n->right != nullptr, "6 must have two children");
    removeBoth(tree, map, 6);
    checkSame(tree, map, "after deleting 6");

    checkSearch(tree, map, 2, false);
    checkSearch(tree, map, 6, false);

    cout << "  After:" << endl;
    tree.Display();
}

// 9
static void testDeleteRoot()
{
    // (a) a tree with only one node
    {
        RedBlackTree tree;
        MapContainer map;
        addBoth(tree, map, 42);
        removeBoth(tree, map, 42);

        expect(tree.IsEmpty(), "the tree must be empty");
        checkSame(tree, map, "after deleting the only node");
    }

    // (b) a bigger tree: delete the root again and again until it is empty
    {
        RedBlackTree tree;
        MapContainer map;
        fillSorted(tree, map, 15);

        while (!tree.IsEmpty())
        {
            int rootValue = tree.GetRoot()->value;

            removeBoth(tree, map, rootValue);

            checkSearch(tree, map, rootValue, false);
            checkSame(tree, map, "after deleting the root " + to_string(rootValue));

            if (!testOk)
                break;
        }
    }
}

// 10
static void testDeleteFromEmpty()
{
    RedBlackTree tree;
    MapContainer map;

    removeBoth(tree, map, 5);       // must do nothing and must not crash

    expect(tree.IsEmpty(), "the tree must still be empty");
    checkSame(tree, map, "after deleting from empty structures");
}

// 11
static void testDuplicates()
{
    RedBlackTree tree;
    MapContainer map;

    addBoth(tree, map, 10);
    addBoth(tree, map, 10);
    addBoth(tree, map, 10);
    addBoth(tree, map, 20);
    addBoth(tree, map, 20);

    expect(tree.Size() == 2, "duplicates must be ignored: size must be 2");
    expect(map.Size() == 2, "map: duplicates must be ignored");
    expect(tree.GetStatistics().allocations == 2, "only 2 nodes must be created");
    expect(tree.GetStatistics().deallocations == 0, "no node must be created and thrown away");
    checkSame(tree, map, "after inserting duplicates");
}

// 12
static void testDeleteNonExisting()
{
    RedBlackTree tree;
    MapContainer map;
    fillSorted(tree, map, 10);

    removeBoth(tree, map, 99);
    removeBoth(tree, map, 0);

    expect(tree.Size() == 10, "size must not change");
    checkSame(tree, map, "after deleting values that do not exist");
}

// 13
static void testSortedInsertion()
{
    int n = 1000;

    // The height of a Red-Black tree is at most 2 * log2(n + 1)
    int k = 0;
    while ((1 << k) < n + 1)
        k++;

    // ascending order
    {
        RedBlackTree tree;
        MapContainer map;

        for (int i = 1; i <= n; i++)
        {
            addBoth(tree, map, i);
            if (!tree.ValidateProperties())
            {
                expect(false, "the tree is invalid after inserting " + to_string(i));
                break;
            }
        }
        checkSame(tree, map, "after 1000 ascending insertions");

        cout << "  Ascending : height = " << tree.Height() << " (limit = " << 2 * k << ")" << endl;
        expect(tree.Height() <= 2 * k, "the tree is too tall");
    }

    // descending order
    {
        RedBlackTree tree;
        MapContainer map;

        for (int i = n; i >= 1; i--)
        {
            addBoth(tree, map, i);
            if (!tree.ValidateProperties())
            {
                expect(false, "the tree is invalid after inserting " + to_string(i));
                break;
            }
        }
        checkSame(tree, map, "after 1000 descending insertions");

        cout << "  Descending: height = " << tree.Height() << " (limit = " << 2 * k << ")" << endl;
        expect(tree.Height() <= 2 * k, "the tree is too tall");
    }
}

// 14
static void testRepeatedInsertDelete()
{
    RedBlackTree tree;
    MapContainer map;

    for (int round = 1; round <= 5 && testOk; round++)
    {
        int i;

        for (i = 1; i <= 200; i++)                  // insert 1..200
            addBoth(tree, map, i);
        checkSame(tree, map, "after inserting, round " + to_string(round));

        for (i = 1; i <= 200; i += 2)               // delete the odd values
        {
            removeBoth(tree, map, i);
            if (!tree.ValidateProperties())
            {
                expect(false, "the tree is invalid while deleting " + to_string(i));
                break;
            }
        }
        checkSame(tree, map, "after deleting the odd values, round " + to_string(round));

        for (i = 1; i <= 200; i++)                  // insert everything again
            addBoth(tree, map, i);
        checkSame(tree, map, "after inserting again, round " + to_string(round));

        for (i = 200; i >= 1; i--)                  // delete everything
        {
            removeBoth(tree, map, i);
            if (!tree.ValidateProperties())
            {
                expect(false, "the tree is invalid while deleting " + to_string(i));
                break;
            }
        }
        expect(tree.IsEmpty() && map.Size() == 0, "both must be empty at the end of the round");
    }
}

// =====================================================================
//  2 extra tests
// =====================================================================

// 15: 20,000 random operations. After EACH one the tree is validated.
static void testRandomOperations()
{
    RedBlackTree tree;
    MapContainer map;
    mt19937 generator(7);

    for (int i = 0; i < 20000; i++)
    {
        int key = (int)(generator() % 300);
        int operation = (int)(generator() % 3);

        if (operation == 0)
            addBoth(tree, map, key);
        else if (operation == 1)
            removeBoth(tree, map, key);
        else
            expect((tree.Search(key) != nullptr) == map.Search(key), "tree and map disagree on a search");

        if (!tree.ValidateProperties() || tree.Size() != map.Size())
        {
            expect(false, "problem detected at operation " + to_string(i));
            break;
        }
    }

    checkSame(tree, map, "at the end of the random operations");
    cout << "  20,000 random operations done, final size = " << tree.Size() << endl;
}

// 16: the counters
static void testStatistics()
{
    // Inserting 10, 5, 15, 3 changes colors like this:
    //   insert 10 : the root changes RED -> BLACK                      (1)
    //   insert 3  : 5 -> BLACK, 15 -> BLACK, 10 -> RED, then 10 -> BLACK (4)
    // Total = 5 recolorings and 0 rotations.
    {
        RedBlackTree tree;
        tree.Insert(10);
        tree.Insert(5);
        tree.Insert(15);
        tree.Insert(3);

        const Statistics& s = tree.GetStatistics();
        expect(s.recolorings == 5, "expected 5 recolorings, got " + to_string(s.recolorings));
        expect(s.TotalRotations() == 0, "no rotation expected");
        expect(s.allocations == 4 && s.currentNodes == 4 && s.peakNodes == 4, "wrong memory counters");
        expect(s.insertComparisons > 0, "insert comparisons must be counted");
        expect(s.searchComparisons == 0 && s.deleteComparisons == 0, "search/delete counters must be 0");
    }

    // Sorted insertion needs rotations. Search and delete have their own counters.
    {
        RedBlackTree tree;
        for (int i = 1; i <= 100; i++)
            tree.Insert(i);

        const Statistics& s = tree.GetStatistics();
        expect(s.leftRotations > 0, "ascending insertion must use left rotations");
        expect(s.allocations == 100, "100 allocations expected");

        unsigned long long insertCmp = s.insertComparisons;
        for (int i = 1; i <= 100; i++)
            tree.Search(i);
        expect(s.searchComparisons > 0, "search comparisons must be counted");
        expect(s.insertComparisons == insertCmp, "a search must not change the insert counter");

        unsigned long long searchCmp = s.searchComparisons;
        for (int i = 1; i <= 100; i++)
            tree.Delete(i);
        expect(s.deleteComparisons > 0, "delete comparisons must be counted");
        expect(s.searchComparisons == searchCmp, "a delete must not change the search counter");
        expect(s.deallocations == 100 && s.currentNodes == 0 && s.peakNodes == 100,
               "wrong memory counters after deleting everything");

        tree.ResetStatistics();
        expect(s.TotalRotations() == 0 && s.recolorings == 0 && s.TotalComparisons() == 0 &&
               s.allocations == 0 && s.deallocations == 0, "Reset must put the counters back to 0");
    }

    // The map counts comparisons too.
    {
        MapContainer map;
        for (int i = 1; i <= 100; i++)
            map.Insert(i);
        expect(map.GetComparisons() > 0, "the map must count comparisons");

        map.ResetComparisons();
        expect(map.GetComparisons() == 0, "ResetComparisons must put the counter back to 0");
    }

    RedBlackTree tree;
    tree.Insert(1);
    tree.Insert(2);
    tree.Insert(3);
    tree.DisplayStatistics();
}

// =====================================================================

bool RunAllTests()
{
    passed = 0;
    failed = 0;

    cout << endl;
    cout << "=====================================================" << endl;
    cout << " TESTS: Red-Black tree and map" << endl;
    cout << "=====================================================" << endl << endl;

    runTest(1,  "Empty tree", testEmptyTree);
    runTest(2,  "Insertion of one element", testInsertOne);
    runTest(3,  "Insertion of multiple elements", testInsertMultiple);
    runTest(4,  "Search for an existing value", testSearchExisting);
    runTest(5,  "Search for a nonexistent value", testSearchNonExisting);
    runTest(6,  "Deletion of a leaf (RED and BLACK)", testDeleteLeaf);
    runTest(7,  "Deletion of a node with one child", testDeleteOneChild);
    runTest(8,  "Deletion of a node with two children", testDeleteTwoChildren);
    runTest(9,  "Deletion of the root", testDeleteRoot);
    runTest(10, "Deletion from an empty tree", testDeleteFromEmpty);
    runTest(11, "Insertion of duplicate values", testDuplicates);
    runTest(12, "Deletion of a nonexistent value", testDeleteNonExisting);
    runTest(13, "Insertion of sorted values", testSortedInsertion);
    runTest(14, "Repeated insertion and deletion", testRepeatedInsertDelete);
    runTest(15, "Extra: 20,000 random operations", testRandomOperations);
    runTest(16, "Extra: statistics counters", testStatistics);

    cout << "=====================================================" << endl;
    cout << " RESULT: " << passed << " passed, " << failed << " failed" << endl;
    cout << (failed == 0 ? " ALL TESTS PASSED" : " SOME TESTS FAILED") << endl;
    cout << "=====================================================" << endl;

    return failed == 0;
}
