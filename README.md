## SAT SOLVER

A SAT solver for CNF formulas using the classical Davis-Putnam-Logemann-Loveland algorithm.
The boolean satisfiability problem is the first shown to be NP-complete, therefore the best known algorithms are still exponential in the worst case, as they are all ultimately brute force.
However, the use of basic propagation rules and heuristics can drastically improve practical performance, especially for real life applications (as opposed to randomly generated instances).

I implemented my SAT solver in C++, using positive integers to index each variable, and stored each clause using a C++ vector of integers.
In each clause, a positive value n represents the literal x_n, while a negative value -n represents the literal (not x_n).
The entire formula is stored as a vector of clauses. The main DPLL routine picks a split state and keeps on propagating until we reach a conflict or we satisfy the formula. Upon reaching a conflict, we call the backtrack routine to modify the state back to the last split.
When we backtrack back to the root, we know that we have completed a thorough search across all formulas and can conclude that the formula is not satisfiable.

My code consists of two alternate implementations: sat-trace, which allows for easy tracing by printing the results of applying various rules, and sat-fast, which is more streamlined and is therefore able to solve larger formulas.
Both employ a greedy heuristic that chooses the variable which occurs the most frequently in the formula.


### Compilation and Execution
gcc is required. To compile and run, enter the following commands:

```
>> make clean
>> make all
>> ./dpll-trace test-file.cnf
>> ./dpll-fast test-file.cnf
```
