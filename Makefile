# make        builds the program (Release mode, -O2: needed for fair timings)
# ./rbt       starts it
CXX      = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra
SOURCES  = main.cpp RedBlackTree.cpp MapContainer.cpp Tests.cpp Benchmark.cpp

rbt: $(SOURCES) *.h
	$(CXX) $(CXXFLAGS) -o rbt $(SOURCES)

clean:
	rm -f rbt rbt.exe benchmark_results.txt
