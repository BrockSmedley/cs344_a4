#pragma once
#include <ctype.h>
#include "normchar.h"

int MODULO = 27;

// returns (a + b) % 26 round-robin style
int mod(int a, int b){
  return (a+b) % MODULO;
}

int demod(int a, int b){
  if (a-b < 0)
    return (a-b) + MODULO;
  else
    return (a-b);
}

// returns ciphered char using modular addition 
int modc(char a, char b){
  int ia = -1, ib = -1;
  // give spaces special value
  if (a == ' ')
    ia = 26;
  if (b == ' ')
    ib = 26;

  a = (char)toupper(a);
  b = (char)toupper(b);

  if (ia == -1)
    ia = normalize_char(a); 
  if (ib == -1)
    ib = normalize_char(b); 

  return mod(ia, ib);
}

// returns deciphered char using modular subtraction
int demodc(char a, char b){
  int ia = normalize_char(a);
  int ib = normalize_char(b); 

  return demod(ia, ib);
}
