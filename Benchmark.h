#ifndef BENCHMARK_H_INCLUDED
#define BENCHMARK_H_INCLUDED

// Compares our Red-Black tree with std::map on 3 kinds of input
// (random, sorted, almost sorted) and 5 sizes (1,000 to 1,000,000).
// Prints the tables and also saves them in benchmark_results.txt
void RunBenchmark();

#endif // BENCHMARK_H_INCLUDED
