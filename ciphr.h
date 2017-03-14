#pragma once
#include <string.h>
#include "newmod.h"
#include <stdio.h>
#include <stdlib.h>

char cipher_char(char messageChar, char keyChar){
  return get_normal(modc(messageChar, keyChar));
}

char* cipher_str(char* message, char* key){
  if (strlen(message) > strlen(key)){
    fprintf(stderr, "message size must be <= key size\n");
    return NULL;
  }
  else{
    int i = 0;
    char* out = malloc(strlen(message) * sizeof(char));
    for (i; i < strlen(message); i++)
      out[i] = cipher_char(message[i], key[i]);
    return out;
  }
}

char decipher_char(char cipherChar, char keyChar){
  return get_normal(demodc((cipherChar), keyChar));
}

char* decipher_str(char* ciphertext, char* key){
  if (strlen(ciphertext) > strlen(key)){
    fprintf(stderr, "cipher size must be <= key size\n");
    return NULL;
  }
  else{
    int i = 0;
    char* out = malloc(strlen(ciphertext) * sizeof(char));
    for (i; i < strlen(ciphertext); i++)
      out[i] = decipher_char(ciphertext[i], key[i]);
    return out;
  }
}
