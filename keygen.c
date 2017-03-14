#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "normchar.h"

int main(int argc, char *argv[]){
  int i;
  time_t t;

  // initialize random number generator
  srand((unsigned)time(&t));
  
  // get size arg
  i = atoi(argv[1]);

  if (i <= 0){
    fprintf(stderr, "Size must be greater than 0.\n");
    exit(1);
  }

  for (i; i > 0; i--){
    printf("%c", get_normal(rand() % 27));
  }
  printf("\n");

  return 0;
}
