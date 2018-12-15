/*
 * An implementation of the DPLL algorithm given a splitting heuristic.
 */

#include "dpll.hpp"
// #include <getopt.h>

static int verbose = 1;
formula_t original_formula;
formula_t curr_formula;
unsigned num_vars, num_clauses;

/* Each variable maps to an array of pointers to clauses it appears in */
// int *positives, *negatives;

/* list of current variable assignments */
int *assignment;

/* stack of current variable assignments */
std::vector<int> decision_stack;
int back_call;

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
    printf("\n");
  }
}

formula_t copy(formula_t f) {
  formula_t new_formula;
  new_formula.resize(f.size());
  for (unsigned i = 0; i < f.size(); i++) {
    new_formula[i].resize(f[i].size());
    copy(f[i].begin(), f[i].end(), new_formula[i].begin());
  }
  return new_formula;
}

void parse_init(FILE *fin) {
  char buf[MAX_LINE];
  char *next_val;
  while (fgets(buf, MAX_LINE, fin)) {
    if (buf[0] == 'c'); // comment line
    else if (buf[0] == 'p') {
      sscanf(buf, "p cnf %d  %d\n", &num_vars, &num_clauses);
      assignment = (int*) malloc(sizeof(int) * (num_vars + 1));
      // positives = (int*) calloc(sizeof(int), (num_vars + 1));
      // negatives = (int*) calloc(sizeof(int), (num_vars + 1));
      for (unsigned i = 0; i < num_vars + 1; i++) {
        /* all variables begin unassigned */
        assignment[i] = -1;
      }
    }
    else if (buf[0] == '%') break;
    else {
      int clause_len = 0;
      for (unsigned i = 0; i < strlen(buf); i++) {
        if (buf[i] == ' ') clause_len++;
      }
      clause_t new_clause;
      int idx = 0;
      next_val = strtok(buf, " ");
      while (idx < clause_len) {
        int val = atoi(next_val);
        new_clause.push_back(val);
        /*
        if (val > 0) {
          positives[val]++;
        }
        else {
          negatives[val]++;
        }
        */
        next_val = strtok(NULL, " ");
        idx++;
      }
      if (!new_clause.empty() && !new_clause.back()) new_clause.pop_back();
      original_formula.push_back(new_clause);
    }
  }
  curr_formula = copy(original_formula);
  print_formula(curr_formula);
}

/*
void unit_propagate(formula_t f) {
  auto it = f.begin();
  while (it != f.end()) {
    if ((*it).size() == 1) {

    }
  }
}
*/

/* if all occurrences are positive or all are negative */
void pure_literal_assign() {

}

// makes assignment and returns 0 if no conflicts, -1 if there are
int setvar(int varnum, int val) {
  assignment[varnum] = val;
  if (val == T) {
    // decision_stack.push_back(varnum);
    return check_conflict(varnum);
  }
  else {
    // decision_stack.push_back(-varnum);
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
    /*
    printf("duplicated\n");
    print_formula(curr_formula);
    print_formula(original_formula);
    */
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
    printf("reset to here");
    print_formula(curr_formula);
    back_call = -prev;
    /*
    decision_stack.push_back(-prev);
    // do this for pending
    setvar(prev, F);
    printf("backtracked to here\n");
    printf("Set x%d to %d\n", prev, F);
    print_formula(curr_formula);
    */
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
    return 1;
  }
  else {
    /* cannot tell if current formula satisfiable */
    return 0;
  }
}

// for now arbitrary
int heuristic() {
  if (back_call) {
    printf("Backtrack call\n");
    // printf("Set x%d to %d\n", -back_call, F);
    int temp = back_call;
    back_call = 0;
    return temp;
  }
  for (unsigned i = 1; i < num_vars + 1; i++) {
    if (assignment[i] == -1) return i;
  }
  return 0;
  /* unit propagation */
  /*
  for (unsigned i = 0; i < f.size(); i++) {
    if (f[i].size() == 1) {

    }
  }
  */
}

bool dpll() {
  while (1) {
    int splitvar = heuristic();
    int abs;
    if (splitvar == 0) return true; // indicates valid and full
    else if (splitvar > 0) abs = splitvar;
    else abs = -splitvar;
/*
    if (verbose) {
      printf("Split at variable x%d\n", -splitvar);
    }
  */
    decision_stack.push_back(splitvar);
    printf("Set x%d to %d\n", abs, abs == splitvar);
    printf("Current depth: %d\n", (int) decision_stack.size());
    int res;
    if (splitvar > 0) res = setvar(splitvar, T);
    else res = setvar(-splitvar, F);
    if (res == -1) {
      printf("Ran into conflict: \n");
      // print_formula(original_formula);
      // print_formula(curr_formula);
      if (!backtrack()) {
        return false;
      }
    }
    else if (res == 0) {
      printf("unknown\n");
      print_formula(curr_formula);
    }
    else {
      printf("known\n");
    }
  }
}

int main(int argc, char **argv) {
  FILE *fin;
  // int c;
/*
  while ((c = getopt(argc, argv, "f:v") != -1)) {
    printf("%d\n", c);
    switch (c) {
      case 'v':
        verbose = 1;
        break;
      case 'f':
        if ((fin = fopen(optarg, "r")) == NULL) {
          fprintf(stderr, "Unable to open file\n");
        }
        break;
      case '?':
        fprintf(stderr, "Usage: %s [-f input_file] [-v]\n", argv[0]);
        return -1;
    }
  }
  */
  if (argc < 2) return -1;
  if ((fin = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open file\n");
  }
  // if (argc >= 3 && !strcmp(argv[2], "-v")) verbose = 1;
  parse_init(fin);
  if (verbose) {
    print_formula(original_formula);
  }

  if (dpll()) {
    printf("SAT\n");
    for (unsigned i = 1; i < num_vars + 1; i++) {
      printf("x%d: %d\n", i, assignment[i]);
    }
  }
  else {
    printf("UNSAT\n");
  }


}
