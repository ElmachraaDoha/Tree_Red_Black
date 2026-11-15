#include "MapContainer.h"

using namespace std;

MapContainer::MapContainer() : comparisonsDone(0), data(CountingLess(&comparisonsDone))
{
}

void MapContainer::Insert(int value)
{
    data.insert(make_pair(value, value));   // a duplicate is ignored
}

bool MapContainer::Search(int value)
{
    return data.find(value) != data.end();
}

void MapContainer::Delete(int value)
{
    data.erase(value);                      // nothing happens if it does not exist
}

int MapContainer::Size()
{
    return (int)data.size();
}

vector<int> MapContainer::Keys()
{
    vector<int> keys;

    for (map<int, int, CountingLess>::iterator it = data.begin(); it != data.end(); ++it)
        keys.push_back(it->first);

    return keys;
}

unsigned long long MapContainer::GetComparisons()
{
    return comparisonsDone;
}

void MapContainer::ResetComparisons()
{
    comparisonsDone = 0;
}
