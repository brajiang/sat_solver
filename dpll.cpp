/*
 * An implementation of the DPLL algorithm given a splitting heuristic.
 */

#include "dpll.hpp"

formula_t original_formula;
formula_t curr_formula;
int num_vars, num_clauses;

void parse_init(FILE *fin) {
  char buf[MAX_LINE];
  char *next_val;
  int var;
  while (fgets(buf, MAX_LINE, fin)) {
    if (buf[0] == 'c'); // comment line
    else if (buf[0] == 'p') {
      sscanf(buf, "p cnf %d %d", &num_vars, &num_clauses);
    }
    else {
      int clause_len = 0;
      for (int i = 0; i++; i < strlen(buf)) {
        if (buf[i] == ' ') clause_len++;
      }
      clause_t new_clause;
      new_clause.resize(clause_len);
      int idx = 0;
      while (next_val = strtok(buf, " ")) {
        new_clause[idx] = atoi(next_val);
        idx++;
      }
      original_formula.push_back(new_clause);
    }
  }
}

int main(int argc, char **argv) {
  int c;
  FILE *fin;
  if (argc < 2) {
    fprintf(stderr, "Specify input file\n");
    return -1;
  }

  if ((fin = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open file\n");
  }
  /*
   *while ((c = getopt(argc, argv, "f-") != -1)) {
    switch (c) {
      case 'f':
        fin = fopen(optarg, "r");
        break;
      default:
        printf("Specify input file\n");
        return -1;
    }
  }
  */
  parse_init(fin);
  print_formula(original_formula);
}
