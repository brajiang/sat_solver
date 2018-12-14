#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#define MAX_LINE 60
#define T 1
#define F 0

/* Represent each clause as array of ints */
typedef std::vector<int> clause_t;
/* Represent formula as array of clauses */
typedef std::vector<clause_t> formula_t;

void print_clause(clause_t c) {
  printf("(");
  for (unsigned i = 0; i < c.size(); i++) {
    int varnum = c[i];
    if (varnum > 0) {
      printf("x%d ", varnum);
    }
    else {
      printf("-x%d ", -varnum);
    }
  }
  printf(")");
}

void print_formula(formula_t f) {
  for (unsigned i = 0; i < f.size(); i++) {
    print_clause(f[i]);
    printf("*\n");
  }
}

void insert_clause(formula_t *formula, int *arr);
void remove_clause(clause_t *clause);

// heuristic?

// splitting?
