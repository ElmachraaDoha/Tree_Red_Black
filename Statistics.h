#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

#include <iostream>

// A sheet of counters. The tree adds 1 to a counter each time
// something happens (a rotation, a recoloring, a comparison...).
struct Statistics
{
    unsigned long long leftRotations = 0;
    unsigned long long rightRotations = 0;

    unsigned long long recolorings = 0;        // only REAL color changes

    unsigned long long insertComparisons = 0;  // 1 comparison = 1 node visited
    unsigned long long searchComparisons = 0;
    unsigned long long deleteComparisons = 0;

    unsigned long long allocations = 0;        // number of "new Node"
    unsigned long long deallocations = 0;      // number of "delete Node"
    unsigned long long currentNodes = 0;       // nodes alive right now
    unsigned long long peakNodes = 0;          // most nodes alive at once

    void Reset()
    {
        leftRotations = rightRotations = recolorings = 0;
        insertComparisons = searchComparisons = deleteComparisons = 0;
        allocations = deallocations = currentNodes = peakNodes = 0;
    }

    unsigned long long TotalRotations() const
    {
        return leftRotations + rightRotations;
    }

    unsigned long long TotalComparisons() const
    {
        return insertComparisons + searchComparisons + deleteComparisons;
    }

    void Display() const
    {
        std::cout << "Rotations   : left = " << leftRotations
                  << ", right = " << rightRotations
                  << ", total = " << TotalRotations() << std::endl;
        std::cout << "Recolorings : " << recolorings << std::endl;
        std::cout << "Comparisons : insert = " << insertComparisons
                  << ", search = " << searchComparisons
                  << ", delete = " << deleteComparisons
                  << ", total = " << TotalComparisons() << std::endl;
        std::cout << "Memory      : allocations = " << allocations
                  << ", deallocations = " << deallocations
                  << ", current = " << currentNodes
                  << ", peak = " << peakNodes << std::endl;
    }
};

#endif // STATISTICS_H_INCLUDED
