#include "geracodigo.h"

int main(int argc, char *argv[]){
  unsigned char codigo[9999];
  int res;

  FILE *arquivo = fopen("teste1.txt", "r");
  if(!arquivo){
    printf("nem abrir essa porra abre");
    exit(1);
  }
  funcp funcaoSB;
  funcaoSB = geraCodigo(arquivo, codigo);
  res = (*funcaoSB)(3);
	printf("%d", res);

  fclose(arquivo);
  return 0;
}