#
# Basic Makefile to compile sat solver
#

CPP = g++
CPPFLAGS= -O2 -std=c++11

all: trace fast

trace: dpll-trace.o
	$(CPP) $(CPPFLAGS) -o dpll-trace dpll-trace.o

fast: dpll-fast.o
	$(CPP) $(CPPFLAGS) -o dpll-fast dpll-fast.o

dpll-trace.o: dpll-trace.cpp dpll.hpp
dpll-fast.o: dpll-fast.cpp dpll.hpp

clean:
	rm -f *~ *.o dpll-trace dpll-fast

