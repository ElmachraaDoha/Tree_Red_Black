#include <iostream>

#include "Tests.h"
#include "Benchmark.h"

using namespace std;

int main()
{
    int choice = -1;

    while (choice != 0)
    {
        cout << endl;
        cout << "===== RED-BLACK TREE PROJECT =====" << endl;
        cout << "1 - Run the tests" << endl;
        cout << "2 - Run the benchmark (about 1 minute)" << endl;
        cout << "0 - Exit" << endl;
        cout << "Your choice: ";

        if (!(cin >> choice))
            break;

        if (choice == 1)
            RunAllTests();
        else if (choice == 2)
            RunBenchmark();
        else if (choice != 0)
            cout << "Please type 1, 2 or 0." << endl;
    }

    return 0;
}
