#pragma once
#include <ctype.h>

int ASCII_DIFF = 65; // subtract 65 to convert ASCII to [0,27)

// returns char based off "normalized" integer
char get_normal(int n){
  if (n == 26)
    return ' ';
  else
    return (char)(n + ASCII_DIFF);
}

// converts char to int [0,27)
int normalize_char(char c){
  if (c == ' ')
    return 26;
  else
    return (int)c - ASCII_DIFF;
}
