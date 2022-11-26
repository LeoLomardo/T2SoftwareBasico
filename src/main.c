#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "geracodigo.h"


void printInstruction(unsigned char first[], int number){
    printf("{");
    for (int i = 0; i < number; i++) {
        printf("%02X", first[i]);
        if (i < (number - 1)){printf(" ");}
    }
    printf("}\n");
}

int main(int argc, char *argv[]){
  FILE *arquivo = fopen("teste5.txt", "r");
  printf("\n\nPrimeiro print\n\n");
  if(!arquivo){
    printf("Erro ao abrir o arquivo");
    exit(1);
  }

  unsigned char *codigo = malloc(700* sizeof(char));
  funcp funcSB;
  
  funcSB  = geracodigo(arquivo, codigo);

  printInstruction(codigo, 700);
  
  fclose(arquivo);
  free(codigo);
  return 0; 
}