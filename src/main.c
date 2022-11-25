#include<stdlib.h>
#include <stdio.h>
#include "geracodigo.h"

int main(int argc, char *argv[]){
  FILE *arquivo = fopen("teste1.txt", "r");
  if(!arquivo){
    printf("nem abrir essa porra abre");
    exit(1);
  }

  unsigned char codigo[1000];
  funcp funcSB;
  int res;
  
  funcSB  = geracodigo(arquivo, codigo);
  res = (*funcSB)(arquivo, codigo);
  printf("%d", res);
  fclose(arquivo);
  
  return 0;
}