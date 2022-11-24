#include<stdlib.h>
#include <stdio.h>
#include "geracodigo.h"

int main(int argc, char *argv[]){
  printf("\n\nPrimeiríssimo print\n\n");
  unsigned char codigo[9999];
  int res;
  printf("\n\nPrimeiro print\n\n");
  FILE *arquivo = fopen("teste1.txt", "r");
  printf("\n\nArquivo aberto\n\n");
  if(!arquivo){
    printf("nem abrir essa porra abre");
    exit(1);
  }
  funcp funcaoSB;
  //funcaoSB = geraCodigo(arquivo, codigo); ERRO ESTÁ NESSA LINHA - PÉSSIMA NOTÍCIA
  res = (*funcaoSB)(3);
	printf("%d", res);

  fclose(arquivo);
  return 0;
}