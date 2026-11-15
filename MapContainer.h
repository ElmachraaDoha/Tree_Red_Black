#ifndef MAPCONTAINER_H_INCLUDED
#define MAPCONTAINER_H_INCLUDED

#include <map>
#include <vector>

// std::map calls this every time it compares two keys.
// We add 1 to a counter, so we can count the comparisons of std::map.
// (This is the only thing we can measure inside std::map.)
struct CountingLess
{
    unsigned long long* counter;

    explicit CountingLess(unsigned long long* c) : counter(c) {}

    bool operator()(int a, int b) const
    {
        (*counter)++;
        return a < b;
    }
};

// std::map "in its own box", with the same three functions as our tree.
class MapContainer
{
    private:

        unsigned long long comparisonsDone;
        std::map<int, int, CountingLess> data;

        MapContainer(const MapContainer&) = delete;
        MapContainer& operator=(const MapContainer&) = delete;

    public:

        MapContainer();

        void Insert(int value);
        bool Search(int value);     // true if the value is in the map
        void Delete(int value);

        int  Size();
        std::vector<int> Keys();    // all values in increasing order

        unsigned long long GetComparisons();
        void ResetComparisons();
};

#endif // MAPCONTAINER_H_INCLUDED
