#include "geracodigo.h"

int main(int argc, char *argv[]) {
  FILE *myfp;
  unsigned char codigo[1];
  funcp funcaoSB;
  int res;

  /* Abre o arquivo fonte */
  if ((myfp = fopen("programa", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  /* compila a fun��o SB */
  funcaoSB = geraCodigo(myfp, codigo);
  fclose(myfp);

  /* chama a fun��o */
  res = (*funcaoSB)(myfp, codigo);  /* passando par�metro apropriados */
  
  return 0;
}