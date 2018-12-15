/* dpll-fast: A basic SAT solver for CNF formulas, intended to be faster than
 * dpll-trace.
 *
 * Brandon Jiang
 * Email: brjiang@andrew.cmu.edu
 * An implementation of the DPLL algorithm with basic splitting heuristics,
 * unit propagation, and pure literal assignment.
 */

#include "dpll.hpp"

formula_t original_formula;
unsigned num_vars, num_clauses;

/* Keep track of positive and negative occurrences of each variable */
int *positives, *negatives;

/* list of current variable assignments */
int *assignment;

/* stack of current variable assignments */
std::vector<int> decision_stack;

/* variable for forced backtracking step */
int back_call;

/*
 * print_clause: Helper function to print out contents of a single CNF clause
 */
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


/*
 * print_formula: Outputs all of the clauses of a CNF formula f.
 */
void print_formula(formula_t f) {
  for (unsigned i = 0; i < f.size(); i++) {
    print_clause(f[i]);
    printf("\n");
  }
}

/*
 * copy: Creates a duplicate of a formula to be used when backtracking to a
 * previous state.
 */
formula_t copy(formula_t f) {
  formula_t new_formula;
  new_formula.resize(f.size());
  for (unsigned i = 0; i < f.size(); i++) {
    new_formula[i].resize(f[i].size());
    copy(f[i].begin(), f[i].end(), new_formula[i].begin());
  }
  return new_formula;
}

/*
 * parse_init: Parses contents of a .cnf file and initializes corresponding
 * data structures that represent the formula.
 */
void parse_init(FILE *fin) {
  char buf[MAX_LINE];
  char *next_val;
  int idx = 0;
  while (fgets(buf, MAX_LINE, fin)) {
    if (buf[0] == 'c'); // comment line
    else if (buf[0] == 'p') {
      sscanf(buf, "p cnf %d %d\n", &num_vars, &num_clauses);
      assignment = (int*) malloc(sizeof(int) * (num_vars + 1));
      positives = (int*) calloc(sizeof(int), (num_vars + 1));
      negatives = (int*) calloc(sizeof(int), (num_vars + 1));
      original_formula.resize(num_clauses);
      for (unsigned i = 0; i < num_vars + 1; i++) {
        /* all variables begin unassigned */
        assignment[i] = -1;
      }
    }
    else if (buf[0] == '%') break;
    else {
      next_val = strtok(buf, " ");
      while (next_val != NULL) {
        int val = atoi(next_val);
        if (val == 0) idx++;
        else {
          if (val > 0) {
            positives[val]++;
          }
          else {
            negatives[-val]++;
          }
          original_formula[idx].push_back(val);
        }
        next_val = strtok(NULL, " ");
      }
    }
  }
}



/*
 * backtrack: After found conflict, reset to last T decision and change to F
 * returns 1 if successful, 0 if exhausted (and thus unsat)
 */
int backtrack() {
  int prev = 0;
  while (!decision_stack.empty() && (prev = decision_stack.back()) < 0) {
      /* unassign values */
      assignment[-prev] = -1;
      decision_stack.pop_back();
  }
  if (prev > 0) {
    decision_stack.pop_back();
    // reset stack
    back_call = -prev;
    return 1;
  }
  else {
    // stack is now empty
    // we have exhausted all
    return 0;
  }
}

// propagates varnum and checks current formula for conflicts
// returns -1 if conflict (i.e. UNSAT)
// returns 0 if not known
// returns 1 if SAT
int check_conflict() {
  formula_t::iterator it1;
  // reset counts
  for (unsigned i = 0; i < num_vars + 1; i++) {
    positives[i] = 0;
    negatives[i] = 0;
  }
  int unknown = 0;
  for (it1 = original_formula.begin(); it1 != original_formula.end(); ) {
    int clause_true = 0;
    int unknown_clause = 0;
    clause_t::iterator it2;
    for (it2 = (*it1).begin(); it2 != (*it1).end(); ) {
      if ((*it2 > 0 && assignment[*it2] == T) ||
          (*it2 < 0 && assignment[-(*it2)] == F)) {
        /* assignment causes entire clause to become true */
        clause_true = 1;
      }
      else if ((*it2 > 0 && assignment[*it2] == F) ||
                (*it2 < 0 && assignment[-(*it2)] == T)) {
        /* this literal is false */
        if ((*it1).size() == 1) {
          // conflict
          return -1;
        }
      }
      else {
        unknown_clause = 1;
        if (assignment[abs(*it2)] == -1) {
          if (*it2 > 0) {
            positives[abs(*it2)]++;
          }
          else {
            negatives[abs(*it2)]++;
          }
        }
      }

      ++it2;
    }
    if (unknown_clause && !clause_true) {
      unknown = 1;
    }
    else if (!unknown_clause && !clause_true) {
      return -1; // all false
    }
    ++it1;
  }
  if (unknown) {
    /* cannot tell if current formula satisfiable */
    return 0;
  }
  else {
    // satisfied
    return 1;
  }
}

/*
 * Heuristic function for selecting the current cheapest
 */
int heuristic() {
  if (back_call) {
    int temp = back_call;
    back_call = 0;
    return temp;
  }

  /* unit propagation */
  for (unsigned i = 0; i < original_formula.size(); i++) {
    if (original_formula[i].size() == 1) {
    }
  }

  /* pure literal elimination */
  for (unsigned i = 1; i < num_vars + 1; i++) {
    if (assignment[i] == -1) {
      if (positives[i] == 0) return -i;
      if (negatives[i] == 0) return i;
    }
  }

  /* greedy choice */
  int greatest = 0;
  int idx = 0;
  for (unsigned i = 1; i < num_vars + 1; i++) {
    if (assignment[i] == -1) {
      if (positives[i] + negatives[i] > greatest) {
        greatest = positives[i] + negatives[i];
        idx = i;
      }
    }
  }
  return idx;
}

/*
 * dpll: Implements splitting and checking, main function that executes
 * recursion of the algorithm.
 */
bool dpll() {
  while (1) {
    int splitvar = heuristic();
    if (splitvar == 0) return true; // indicates valid and full

    decision_stack.push_back(splitvar);
    int res;
    if (splitvar > 0) assignment[splitvar] = T;
    else assignment[-splitvar] = F;

    res = check_conflict();
    if (res == -1) {
      if (!backtrack()) {
        return false;
      }
    }
  }
}

/* Main execution routine */
int main(int argc, char **argv) {
  FILE *fin;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [input_file]\n", argv[0]);
    return -1;
  }
  if ((fin = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open file\n");
  }
  parse_init(fin);

  if (dpll()) {
    printf("SAT\n");
    for (unsigned i = 1; i < num_vars + 1; i++) {
      printf("x%d: %d\n", i, assignment[i]);
    }
  }
  else {
    printf("UNSAT\n");
  }
  free(assignment);
  free(positives);
  free(negatives);
  return 0;
}
