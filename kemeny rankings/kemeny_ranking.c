/*
kemeny_ranking.c
Kyumin Kim
04.09.2013

Usage: kemeny_ranking [filename]
If filename is not provided, kemeny_ranking will read from stdin by default.

Input format:
First line consists of a single number, <number of candidates>
Every line after that consists of 3 numbers: a b c -- which would
indicate that there are c more people who prefer a to b than there
are people who prefer b to a.
Any edge unspecified in the input is presumed to be zero.

Output:
Single line consisting of the kemeny ranking,
and also the score (lowest possible penalty).
If there are multiple rankings that achieve the best possible score,
the one that comes first lexicographically is printed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define MAX_NUMBER_OF_CANDIDATES 100
int number_of_candidates;
int weight[MAX_NUMBER_OF_CANDIDATES][MAX_NUMBER_OF_CANDIDATES];
int ranking[MAX_NUMBER_OF_CANDIDATES];
void fatal(char* format, ...); 
void read_input(FILE* fin); /* fill in weight */
void initialize_ranking(); /* set ranking = {0, 1, 2, ... n-1} */
int  next_ranking_permutation();
void print_ranking(); /* for debug */
void print_all_ranking_permutations(); /* for debug */
int  ranking_penalty(); /* penalty of current ranking */
void find_kemeny_ranking();

int main(int argc, char** argv) {
  FILE * fin;

  if (argc == 2) {
    if (!(fin = fopen(argv[1], "r"))) {
      fatal("input file '%s' does not exist", argv[1]);
    }
  }
  else           fin = stdin;

  read_input(fin);
  find_kemeny_ranking();
  printf("ranking = ");print_ranking(); printf("\n");
  printf("score   = %d\n", ranking_penalty());
  
  return 0;
}

void fatal(char* format, ...) {
  va_list argptr;
  
  printf("FATAL ERROR:\n  ");
  va_start(argptr, format);
  vfprintf(stdout, format, argptr);
  va_end(argptr);
  printf("\n");
  
  exit(1);
}

void read_input(FILE* fin) {
  int a, b, c;
  fscanf(fin, "%d", &number_of_candidates);
  for (a = 0; a < number_of_candidates; a++) {
    for (b = 0; b < number_of_candidates; b++) {
      weight[a][b] = 0;
    }
  }
  
  while (fscanf(fin,"%d%d%d",&a,&b,&c)!=EOF) {
    if (a >= number_of_candidates || a < 0)
      fatal("candidate index out of bounds %d", a);
    if (b >= number_of_candidates || b < 0)
      fatal("candidate index out of bounds %d", b);
    if (c < 0)
      fatal("negative edge weight %d", c);
    weight[b][a] = c;
  }
  
  for (a = 0; a < number_of_candidates; a++) {
    for (b = 0; b < number_of_candidates; b++) {
      if (weight[a][b] > 0 && weight[b][a] > 0) {
        fatal("candidates %d and %d connected bidirectionally", a, b);
      }
    }
  }
}

void initialize_ranking() {
  int i;
  for (i = 0; i < number_of_candidates; i++) ranking[i] = i;
}

int next_ranking_permutation() {
  int i, j, k, l, m;
  for (i = number_of_candidates-2; i >= 0; i--) {
    if (ranking[i] < ranking[i+1]) break;
  }
  if (i < 0) return 0;
  for (j = number_of_candidates-1; j > i; j--) {
    if (ranking[i] < ranking[j]) break;
  }
  k = ranking[i];
  ranking[i] = ranking[j];
  ranking[j] = k;
  l = (number_of_candidates-(i+1))/2;
  for (m = 0; m < l; m++) {
    k = ranking[i+1+m];
    ranking[i+1+m] = ranking[number_of_candidates-1-m];
    ranking[number_of_candidates-1-m] = k;
  }
  return 1;
}

void print_ranking() {
  int i;
  for (i = 0; i < number_of_candidates; i++) printf("%5d", ranking[i]);
}

void print_all_ranking_permutations() {
  int i;
  i = 0; initialize_ranking();
  do {
    printf("%5d:", i++); print_ranking(); printf("\n");
  } while (next_ranking_permutation());
}

int ranking_penalty() {
  int i, j, p;
  p = 0;
  for (i = 0; i < number_of_candidates; i++) {
    for (j = i+1; j < number_of_candidates; j++) {
      p += weight[ranking[i]][ranking[j]];
    }
  }
  return p;
}

void find_kemeny_ranking() {
  int candidate[MAX_NUMBER_OF_CANDIDATES];
  int score, s;
  initialize_ranking();
  score = ranking_penalty();
  memcpy(candidate, ranking, sizeof(candidate));
  do {
    s = ranking_penalty();
    if (s < score) {
      score = s;
      memcpy(candidate,ranking,sizeof(candidate));
    }
  } while (next_ranking_permutation());
  memcpy(ranking, candidate, sizeof(candidate));
}
