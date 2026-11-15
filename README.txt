RED-BLACK TREE PROJECT
======================

THE FILES (one job each)
------------------------
Node.h              One box of the tree: value, color, left, right, parent.
RedBlackTree.h/.cpp YOUR tree: Insert, Search, Delete, Display,
                    ValidateProperties (checks the 5 rules).
MapContainer.h/.cpp std::map in its own box: Insert, Search, Delete.
Statistics.h        The counters (rotations, recolorings, comparisons, memory).
Tests.cpp/.h        16 tests (the 14 asked + 2 extra). Each prints PASS or FAIL.
Benchmark.cpp/.h    Times the tree and the map, prints the tables.
main.cpp            The menu.
Makefile            Build file.

HOW TO COMPILE
--------------
Terminal (Linux / macOS / MinGW):

    make
    ./rbt

or without make:

    g++ -std=c++11 -O2 -o rbt main.cpp RedBlackTree.cpp MapContainer.cpp Tests.cpp Benchmark.cpp

Code::Blocks:
  1. New empty C++ project, add ALL the .h and .cpp files.
  2. Build options: tick "-std=c++11" and "-O2", and build in Release mode.
     (Debug mode is MUCH slower: never use it for the benchmark.)

THE MENU
--------
1 - Run the tests      (about 2 seconds)
2 - Run the benchmark  (about 1 minute; also saves benchmark_results.txt)
0 - Exit

Run option 1 first. Run option 2 only when all tests say PASS.

IMPORTANT CHOICES (good for the report)
---------------------------------------
* Duplicate values are ignored (same as std::map).
* nullptr is used for NIL leaves, and they count as BLACK.
* 1 comparison = 1 node visited while looking for a key.
* A recoloring is counted only if the color REALLY changes.
* std::map hides its inside: we can count its comparisons (with a counting
  comparator) but NOT its rotations, recolorings or allocations.
