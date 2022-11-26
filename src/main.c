#include<stdlib.h>
#include <stdio.h>
#include "geracodigo.h"
#include <stdio.h>

int main(int argc, char *argv[]){
  FILE *arquivo = fopen("teste1.txt", "r");
  printf("\n\nPrimeiro print\n\n");
  if(!arquivo){
    printf("Erro ao abrir o arquivo");
    exit(1);
  }

  unsigned char codigo[1000];
  funcp funcSB;
  int res;
  //funcSB  = geracodigo(arquivo, codigo);
  res = (*funcSB)(arquivo, codigo);
  printf("%d", res);
  printf("\n\nSegundo print\n\n");
  for (int i=0; i<1000; i++){
    printf("%d", codigo[i]);
  }
  fclose(arquivo);
  
  return 0;
}