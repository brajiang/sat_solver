CPP = g++
CPPFLAGS= -Wall -Wextra

dpll: dpll.o
	$(CPP) $(CPPFLAGS) -o dpll dpll.o

dpll.o: dpll.cpp dpll.hpp

clean:
	rm -f *~ *.o dpll

