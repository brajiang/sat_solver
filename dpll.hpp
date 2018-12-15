/*
 * Header file for sat solver.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <getopt.h>
#define MAX_LINE 100
#define T 1
#define F 0

/* Represent each clause as array of ints */
typedef std::vector<int> clause_t;

/* Represent formula as array of clauses */
typedef std::vector<clause_t> formula_t;

/*
 * print_clause: Helper function to print out contents of a single CNF clause
 */
void print_clause(clause_t c);

/*
 * print_formula: Outputs all of the clauses of a CNF formula f.
 */
void print_formula(formula_t f);

/*
 * parse_init: Parses contents of a .cnf file and initializes corresponding
 * data structures that represent the formula.
 */
void parse_init(FILE *fin);

/*
 * check_conflict: propagates varnum and checks current formula for conflicts
 * returns -1 if conflict (i.e. UNSAT)
 * returns 0 if not known
 * returns 1 if SAT
 */
int check_conflict(int varnum);

/*
 * copy: Creates a duplicate of a formula to be used when backtracking to a
 * previous state.
 */
formula_t copy(formula_t);
