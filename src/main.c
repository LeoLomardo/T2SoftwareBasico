#include<stdlib.h>
#include <stdio.h>
#include "geracodigo.h"

int main(int argc, char *argv[]){
  FILE *arquivo = fopen("teste1.txt", "r");
  if(!arquivo){
    printf("nem abrir essa porra abre");
    exit(1);
  }

  unsigned char codigo[9999];
  funcp funcLBS;
  int res;
  
  funcLBS  = geracodigo(arquivo, codigo);

  fclose(arquivo);

  return 0;
}