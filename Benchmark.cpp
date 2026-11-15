#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Benchmark.h"
#include "RedBlackTree.h"
#include "MapContainer.h"

using namespace std;

// =====================================================================
//  How the benchmark works
//  1. All the data is created BEFORE any stopwatch starts.
//  2. The SAME keys go into the tree and into the map
//     (same insertion order, same search keys, same deletion order).
//  3. Insertion, search and deletion each have their own stopwatch.
//  4. Nothing is printed while a stopwatch is running.
//  5. Everything is repeated several times and we keep the average.
//  6. Before timing, one untimed run checks that the tree is valid.
// =====================================================================

typedef chrono::steady_clock Clock;

// Everything we measure for one input (one type, one size)
struct Result
{
    int n;
    bool valid;                         // the tree passed the check

    double treeInsert, treeSearch, treeDelete;     // milliseconds (average)
    double mapInsert, mapSearch, mapDelete;

    Statistics treeStats;               // counters of our tree
    unsigned long long mapInsertCmp, mapSearchCmp, mapDeleteCmp;   // comparisons of the map
};

// ---------------------------------------------------------------------
//  Creating the input data
// ---------------------------------------------------------------------

// 1, 2, 3, ..., n
static vector<int> makeSorted(int n)
{
    vector<int> v(n);
    for (int i = 0; i < n; i++)
        v[i] = i + 1;
    return v;
}

// The same numbers in a random order
static vector<int> makeShuffled(const vector<int>& values, unsigned int seed)
{
    vector<int> copy = values;
    mt19937 generator(seed);
    shuffle(copy.begin(), copy.end(), generator);
    return copy;
}

// Sorted numbers where 1% of them are swapped with a random position
static vector<int> makeAlmostSorted(int n, unsigned int seed)
{
    vector<int> v = makeSorted(n);
    mt19937 generator(seed);

    int swaps = n / 100;
    if (swaps < 1)
        swaps = 1;

    for (int i = 0; i < swaps; i++)
    {
        int a = (int)(generator() % n);
        int b = (int)(generator() % n);
        swap(v[a], v[b]);
    }
    return v;
}

// ---------------------------------------------------------------------
//  Small helpers
// ---------------------------------------------------------------------

static double milliseconds(Clock::time_point start, Clock::time_point stop)
{
    return chrono::duration<double, milli>(stop - start).count();
}

// 1234567 -> "1,234,567"
static string withCommas(unsigned long long number)
{
    string text = to_string(number);
    string result;
    int digits = 0;

    for (int i = (int)text.size() - 1; i >= 0; i--)
    {
        result = text[i] + result;
        digits++;
        if (digits % 3 == 0 && i > 0)
            result = "," + result;
    }
    return result;
}

// ---------------------------------------------------------------------
//  Measuring one input
// ---------------------------------------------------------------------

static Result measure(const vector<int>& insertKeys,
                      const vector<int>& searchKeys,
                      const vector<int>& deleteKeys)
{
    Result r;
    r.n = (int)insertKeys.size();
    r.valid = true;
    r.treeInsert = r.treeSearch = r.treeDelete = 0;
    r.mapInsert = r.mapSearch = r.mapDelete = 0;
    r.mapInsertCmp = r.mapSearchCmp = r.mapDeleteCmp = 0;

    size_t i;

    // ---- Untimed check: is the tree correct on this input? ----
    {
        RedBlackTree tree;

        for (i = 0; i < insertKeys.size(); i++)
            tree.Insert(insertKeys[i]);
        if (!tree.ValidateProperties() || tree.Size() != r.n)
            r.valid = false;

        for (i = 0; i < deleteKeys.size() / 2; i++)
            tree.Delete(deleteKeys[i]);
        if (!tree.ValidateProperties())
            r.valid = false;

        for (i = deleteKeys.size() / 2; i < deleteKeys.size(); i++)
            tree.Delete(deleteKeys[i]);
        if (!tree.IsEmpty() || !tree.ValidateProperties())
            r.valid = false;
    }

    if (!r.valid)
        return r;       // we never time a tree that is not correct

    // ---- Timed runs ----
    int repetitions = (r.n <= 10000) ? 5 : 3;

    for (int rep = 0; rep < repetitions; rep++)
    {
        // ----- Our Red-Black tree -----
        {
            RedBlackTree tree;
            unsigned long long found = 0;
            Clock::time_point start, stop;

            start = Clock::now();
            for (i = 0; i < insertKeys.size(); i++)
                tree.Insert(insertKeys[i]);
            stop = Clock::now();
            r.treeInsert += milliseconds(start, stop);

            start = Clock::now();
            for (i = 0; i < searchKeys.size(); i++)
                if (tree.Search(searchKeys[i]) != nullptr)
                    found++;
            stop = Clock::now();
            r.treeSearch += milliseconds(start, stop);

            start = Clock::now();
            for (i = 0; i < deleteKeys.size(); i++)
                tree.Delete(deleteKeys[i]);
            stop = Clock::now();
            r.treeDelete += milliseconds(start, stop);

            if (found != searchKeys.size())
                r.valid = false;

            r.treeStats = tree.GetStatistics();     // same every repetition
        }

        // ----- std::map -----
        {
            MapContainer map;
            unsigned long long found = 0;
            Clock::time_point start, stop;

            start = Clock::now();
            for (i = 0; i < insertKeys.size(); i++)
                map.Insert(insertKeys[i]);
            stop = Clock::now();
            r.mapInsert += milliseconds(start, stop);
            r.mapInsertCmp = map.GetComparisons();
            map.ResetComparisons();

            start = Clock::now();
            for (i = 0; i < searchKeys.size(); i++)
                if (map.Search(searchKeys[i]))
                    found++;
            stop = Clock::now();
            r.mapSearch += milliseconds(start, stop);
            r.mapSearchCmp = map.GetComparisons();
            map.ResetComparisons();

            start = Clock::now();
            for (i = 0; i < deleteKeys.size(); i++)
                map.Delete(deleteKeys[i]);
            stop = Clock::now();
            r.mapDelete += milliseconds(start, stop);
            r.mapDeleteCmp = map.GetComparisons();

            if (found != searchKeys.size() || map.Size() != 0)
                r.valid = false;
        }
    }

    // Average
    r.treeInsert /= repetitions;
    r.treeSearch /= repetitions;
    r.treeDelete /= repetitions;
    r.mapInsert /= repetitions;
    r.mapSearch /= repetitions;
    r.mapDelete /= repetitions;

    return r;
}

// ---------------------------------------------------------------------
//  Printing the tables (on the screen and in the file)
// ---------------------------------------------------------------------

static void printTables(ostream& out, const string& title, const vector<Result>& results)
{
    size_t k;

    out << "---------------------------------------------------------------------" << endl;
    out << " " << title << endl;
    out << "---------------------------------------------------------------------" << endl << endl;

    // ---- Table 1: times ----
    out << "Time (average, in milliseconds)" << endl;
    out << "| " << setw(10) << "N" << " | " << left << setw(14) << "Implementation" << right
        << " | " << setw(14) << "Insertion (ms)"
        << " | " << setw(12) << "Search (ms)"
        << " | " << setw(13) << "Deletion (ms)" << " |" << endl;
    out << "|-----------:|:---------------|---------------:|-------------:|--------------:|" << endl;

    out << fixed << setprecision(3);
    for (k = 0; k < results.size(); k++)
    {
        const Result& r = results[k];
        if (!r.valid)
        {
            out << "| " << setw(10) << withCommas(r.n) << " | TREE CHECK FAILED: no timing" << endl;
            continue;
        }
        out << "| " << setw(10) << withCommas(r.n) << " | " << left << setw(14) << "RedBlackTree" << right
            << " | " << setw(14) << r.treeInsert << " | " << setw(12) << r.treeSearch
            << " | " << setw(13) << r.treeDelete << " |" << endl;
        out << "| " << setw(10) << withCommas(r.n) << " | " << left << setw(14) << "std::map" << right
            << " | " << setw(14) << r.mapInsert << " | " << setw(12) << r.mapSearch
            << " | " << setw(13) << r.mapDelete << " |" << endl;
    }
    out << endl;

    // ---- Table 2: counters of OUR tree ----
    out << "Counters of RedBlackTree (insert + search + delete together)" << endl;
    out << "| " << setw(10) << "N"
        << " | " << setw(15) << "Left Rotations"
        << " | " << setw(15) << "Right Rotations"
        << " | " << setw(12) << "Recoloring"
        << " | " << setw(14) << "Comparisons"
        << " | " << setw(12) << "Allocations"
        << " | " << setw(10) << "Peak nodes" << " |" << endl;
    out << "|-----------:|----------------:|----------------:|-------------:|---------------:|-------------:|-----------:|" << endl;

    for (k = 0; k < results.size(); k++)
    {
        const Result& r = results[k];
        if (!r.valid)
            continue;
        out << "| " << setw(10) << withCommas(r.n)
            << " | " << setw(15) << withCommas(r.treeStats.leftRotations)
            << " | " << setw(15) << withCommas(r.treeStats.rightRotations)
            << " | " << setw(12) << withCommas(r.treeStats.recolorings)
            << " | " << setw(14) << withCommas(r.treeStats.TotalComparisons())
            << " | " << setw(12) << withCommas(r.treeStats.allocations)
            << " | " << setw(10) << withCommas(r.treeStats.peakNodes) << " |" << endl;
    }
    out << endl;

    // ---- Table 3: comparisons, tree vs map ----
    out << "Comparisons: RedBlackTree vs std::map" << endl;
    out << "| " << setw(10) << "N"
        << " | " << setw(12) << "Tree insert"
        << " | " << setw(12) << "Map insert"
        << " | " << setw(12) << "Tree search"
        << " | " << setw(12) << "Map search"
        << " | " << setw(12) << "Tree delete"
        << " | " << setw(12) << "Map delete" << " |" << endl;
    out << "|-----------:|-------------:|-------------:|-------------:|-------------:|-------------:|-------------:|" << endl;

    for (k = 0; k < results.size(); k++)
    {
        const Result& r = results[k];
        if (!r.valid)
            continue;
        out << "| " << setw(10) << withCommas(r.n)
            << " | " << setw(12) << withCommas(r.treeStats.insertComparisons)
            << " | " << setw(12) << withCommas(r.mapInsertCmp)
            << " | " << setw(12) << withCommas(r.treeStats.searchComparisons)
            << " | " << setw(12) << withCommas(r.mapSearchCmp)
            << " | " << setw(12) << withCommas(r.treeStats.deleteComparisons)
            << " | " << setw(12) << withCommas(r.mapDeleteCmp) << " |" << endl;
    }
    out << endl;
}

static void printReport(ostream& out, const vector<string>& titles, const vector<vector<Result> >& all)
{
    out << "=====================================================================" << endl;
    out << " BENCHMARK: our RedBlackTree vs std::map" << endl;
    out << "=====================================================================" << endl;
    out << "Same keys, same insertion order, same searches, same deletions." << endl;
    out << "Search = N successful searches. Delete = all N keys in random order." << endl;
    out << "Repetitions: 5 for N <= 10,000, otherwise 3. Times are averages." << endl;
    out << "sizeof(Node) = " << sizeof(Node) << " bytes (our tree: peak memory is about"
        << " peak nodes x " << sizeof(Node) << " bytes)." << endl << endl;

    for (size_t t = 0; t < all.size(); t++)
        printTables(out, titles[t], all[t]);

    out << "=====================================================================" << endl;
    out << " NOTES" << endl;
    out << "=====================================================================" << endl;
    out << "* Rotations, recolorings and memory allocations can only be counted in" << endl;
    out << "  our tree. std::map hides its inside (the C++ standard does not even" << endl;
    out << "  say it must be a Red-Black tree), so we do not show them for std::map." << endl;
    out << "* Comparisons: tree = 1 per node visited. map = number of calls of its" << endl;
    out << "  comparator. They are close, but not identical (erase(key) in std::map" << endl;
    out << "  calls the comparator more often)." << endl;
    out << "* Theory: search, insertion and deletion are O(log n) in both, because" << endl;
    out << "  the height of a Red-Black tree is at most 2 * log2(n + 1)." << endl;
}

// ---------------------------------------------------------------------
//  The benchmark itself
// ---------------------------------------------------------------------

void RunBenchmark()
{
    int sizes[] = {1000, 10000, 100000, 500000, 1000000};

    vector<string> titles;
    titles.push_back("TEST A: RANDOM NUMBERS");
    titles.push_back("TEST B: SORTED NUMBERS");
    titles.push_back("TEST C: ALMOST-SORTED NUMBERS (1% swapped)");

    vector<vector<Result> > all(3);

    cout << endl << "Benchmark running... nothing is printed while timing." << endl;

    for (int type = 0; type < 3; type++)
    {
        for (int s = 0; s < 5; s++)
        {
            int n = sizes[s];
            cout << "  " << titles[type] << " - N = " << withCommas(n) << endl;

            // 1) Create ALL the data first (never inside a stopwatch)
            vector<int> sorted = makeSorted(n);
            vector<int> insertKeys;

            if (type == 0)
                insertKeys = makeShuffled(sorted, 12345);
            else if (type == 1)
                insertKeys = sorted;
            else
                insertKeys = makeAlmostSorted(n, 12345);

            vector<int> searchKeys = makeShuffled(sorted, 101);
            vector<int> deleteKeys = makeShuffled(sorted, 202);

            // 2) Measure
            all[type].push_back(measure(insertKeys, searchKeys, deleteKeys));
        }
    }

    cout << endl;
    printReport(cout, titles, all);

    ofstream file("benchmark_results.txt");
    if (file)
    {
        printReport(file, titles, all);
        cout << endl << "The tables were also saved in benchmark_results.txt" << endl;
    }
}
