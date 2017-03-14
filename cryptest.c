#include "ciphr.h"
#include "newmod.h"
#include "normchar.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
  char key[50];
  char* plaintext = "THE RED GOOSE FUCKS UR BITCH AT MIDNIGHT STOP";
  int fd_key = open("key50.key", O_RDONLY);
  read(fd_key, key, 50);

  printf("plaintext: %s\nkey: %s\n", plaintext, key);

  char* cipher = cipher_str(plaintext, key);
  printf("cipher: %s\n", cipher);

  char* decipher = decipher_str(cipher, key);
  printf("decipher: %s\n", decipher);
}
