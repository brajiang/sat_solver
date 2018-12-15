/* dpll-trace: A basic SAT solver for CNF formulas, provides a trace of the
 * algorithm's execution. Ideal for formulas with fewer than 20 variables.
 *
 * Brandon Jiang
 * Email: brjiang@andrew.cmu.edu
 * An implementation of the DPLL algorithm with basic splitting heuristics,
 * unit propagation, and pure literal assignment.
 */

#include "dpll.hpp"

formula_t original_formula;
formula_t curr_formula;
unsigned num_vars, num_clauses;

/* list of  current variable assignments */
int *assignment;

/* stack of current variable assignments for backtracking */
std::vector<int> decision_stack;

/* forced backtrack call (when conflict occurs) */
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
    if (i != 0 && i % 10 == 0) printf("\n");
  }
  printf("\n");
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
          original_formula[idx].push_back(val);
        }
        next_val = strtok(NULL, " ");
      }
    }
  }
  curr_formula = copy(original_formula);
}


/*
 * setvar: makes assignment and returns 0 if no conflicts, -1 if there are
 */
int setvar(int varnum, int val) {
  assignment[varnum] = val;
  if (val == T) {
    return check_conflict(varnum);
  }
  else {
    return check_conflict(-varnum);
  }
}

/* found conflict, reset to last T decision and change to F */
/* returns 1 if successful, 0 if exhausted */
int backtrack() {
  int prev;
  while (!decision_stack.empty() && (prev = decision_stack.back()) < 0) {
      /* unassign values */
      assignment[-prev] = -1;
      decision_stack.pop_back();
  }

  if (prev > 0) {
    decision_stack.pop_back();
    curr_formula = copy(original_formula);
    // reset to original
    for (unsigned i = 0; i < decision_stack.size(); i++) {
      int val = decision_stack[i];
      if (val > 0) {
        setvar(val, T);
      }
      else {
        setvar(-val, F);
      }
    }
    printf("Backtracked to state:\n");
    print_formula(curr_formula);
    back_call = -prev;
    return 1;
  }
  else {
    // stack is now empty
    return 0;
  }
}

/*
 * check_conflict: propagates varnum and checks current formula for conflicts
 * returns -1 if conflict (i.e. UNSAT)
 * returns 0 if not known
 * returns 1 if SAT
 */
int check_conflict(int var_assign) {
  formula_t::iterator it1;
  for (it1 = curr_formula.begin(); it1 != curr_formula.end(); ) {
    int clause_true = 0;
    clause_t::iterator it2;
    for (it2 = (*it1).begin(); it2 != (*it1).end(); ) {
      if (*it2 == var_assign) {
        /* assignment causes entire clause to become true */
        clause_true = 1;
        break;
      }
      else if (*it2 == -var_assign) {
        /* this literal is false */
        if ((*it1).size() == 1) {
          // conflict
          return -1;
        }
        else {
          // remove
          it2 = (*it1).erase(it2);
        }
      }
      else {
        ++it2;
      }
    }
    /* check if entire clause is true */
    if (clause_true) {
      it1 = curr_formula.erase(it1);
    }
    else {
      ++it1;
    }
  }
  if (curr_formula.empty()) {
    /* all clauses resolved */
    return 1;
  }
  else {
    /* cannot tell if current formula satisfiable */
    return 0;
  }
}

/*
 * Heuristic function for selecting the variable with most occurrences
 */
int heuristic() {
  if (back_call) {
    int temp = back_call;
    back_call = 0;
    return temp;
  }

  /* unit propagation */
  for (unsigned i = 0; i < curr_formula.size(); i++) {
    if (curr_formula[i].size() == 1) {
      return curr_formula[i][0];
    }
  }

  for (unsigned i = 1; i < num_vars + 1; i++) {
    if (assignment[i] == -1) return i;
  }
  return 0;
}

/*
 * dpll: Implements splitting and checking, main function that executes
 * recursion of the algorithm.
 */
bool dpll() {
  while (1) {
    int splitvar = heuristic();
    int abs;
    if (splitvar == 0) return true; // indicates valid and full
    else if (splitvar > 0) abs = splitvar;
    else abs = -splitvar;

    decision_stack.push_back(splitvar);
    printf("Splitting case at x%d, set to %d\n", abs, abs == splitvar);
    int res;
    if (splitvar > 0) res = setvar(splitvar, T);
    else res = setvar(-splitvar, F);
    if (res == -1) {
      printf("Conflict! Backtracking...\n");
      if (!backtrack()) {
        return false;
      }
    }
    else if (res == 0) {
      printf("Unable to resolve: \n");
      print_formula(curr_formula);
    }
    else {
      printf("Resolved.\n");
    }
  }
}

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
  return 0;
}
