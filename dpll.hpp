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

void print_clause(clause_t c);
void print_formula(formula_t f);
void insert_clause(formula_t *formula, int *arr);
void remove_clause(clause_t *clause);

int check_conflict(int varnum);

formula_t copy(formula_t);

// heuristic?

// splitting?
